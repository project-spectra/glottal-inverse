#include "amgif.h"
#include "linalg.h"
#include "constants.h"

#include <iostream>
#include <chrono>


using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;

using ListCmu = const vector<mat_operator>;
using VecPair = pair<gsl_vector *, gsl_vector *>;

// macros to define static (and allocated on stack)
// vectors and matrices to avoid reallocation
// whenever the function is called.

#define _static_array(name, length) static double data_##name[length]
#define _static_vector_view(name, length) static auto view_##name = gsl_vector_view_array(data_##name, length)
#define _static_matrix_view(name, n1, n2) static auto view_##name = gsl_matrix_view_array(data_##name, n1, n2)

#define static_vector(name, length) \
    _static_array(name, length); \
    _static_vector_view(name, length); \
    static auto name = &view_##name.vector; \

#define static_matrix(name, n1, n2) \
    _static_array(name, n1 * n2); \
    _static_matrix_view(name, n1, n2); \
    static auto name = &view_##name.matrix; \

static constexpr size_t length = WINDOW_LENGTH;

static double errorDistance(const gsl_vector *a, const gsl_vector *b);

static void computeMatA(gsl_matrix *A, const gsl_matrix *yT, ListCmu& C);
static void solveForX(gsl_permutation *perm, const gsl_matrix *A, gsl_vector *xHat, const gsl_matrix *L, const gsl_vector *dd, const double alpha);

static void computeMatB(gsl_matrix *B, const gsl_vector *x, ListCmu& C);
static void solveForY(gsl_permutation *perm, const gsl_matrix* B, gsl_vector *yHat, const gsl_vector *ye, const gsl_vector *dd, const double beta, const double tau);

static void computeD(const gsl_matrix *A, const gsl_matrix *B, const gsl_vector *x, const gsl_vector *y, gsl_vector *d, double &dist);

VecPair computeAMGIF(
        ListCmu& C,
        gsl_vector *md, // source
        gsl_vector *pe, // charac
        mat_operator& L,
        double alpha,
        double beta,
        double tau,
        double eps
) {

    // Permutation for Cholesky decompostion
    static auto perm = shared_ptr<gsl_permutation>(
            gsl_permutation_alloc(length),
            &gsl_permutation_free
    );

    // Results: x, y, d
    static_vector(x, length);
    static_vector(y, length);
    static_vector(d, length);
   
    // dd = F(md)
    static_vector(dd, length);
    coords(md->data, data_dd);

    // ye = F(pe)
    static_vector(ye, length);
    coords(pe->data, data_ye);

    // yHat = ye
    static_vector(yHat, length);
    gsl_vector_memcpy(yHat, ye);

    static_vector(xHat, length);
 
    // A, B
    static_matrix(A, length, length);
    static_matrix(B, length, length);

    // column matrix view of y
    static auto view_yT = gsl_matrix_view_vector(y, 1, length);
    static auto yT = &view_yT.matrix;

    double errForwBack;
    double errSignal;

    size_t iters = 1;

    do {
        gsl_vector_memcpy(y, yHat);  // y = yHat
        computeMatA(A, yT, C);
        
        // TODO: find alpha parameter
        solveForX(perm.get(), A, xHat, L.get(), dd, alpha);  // estimated xHat

        gsl_vector_memcpy(x, xHat);  // x = xHat
        computeMatB(B, x, C);

        // TODO: find beta parameter
        solveForY(perm.get(), B, yHat, ye, dd, beta, tau);  // estimated yHat

        // Test for convergence:  Ax = By = d ~ dd
        computeD(A, B, x, y, d, errForwBack);
        
        errSignal = errorDistance(d, dd);

        //std::cout << iters << ": Ax~By = " << errForwBack << "\t d~dd = " << errSignal << std::endl;

        ++iters;
    } while (errSignal > eps && iters <= MAX_ITER);

    // d = F(m) : wavelet coordinates for the speech function
    // x = F(f) : wavelet coordinates for the input function
    // y = F(p) : wavelet coordinates for the caracteristic function
    
    auto m = gsl_vector_alloc(length);
    auto f = gsl_vector_alloc(length);
    auto p = gsl_vector_alloc(length);

    uncoords(d->data, m->data);
    uncoords(x->data, f->data);
    uncoords(y->data, p->data);

    gsl_vector_fprintf(stdout, dd, "%g");

    /*  Just for testing, what's the residual between m and md  */
    std::cout << "Residual (signal):  " << errorDistance(m, md) << std::endl;
    std::cout << "Residual (wavelet): " << errorDistance(d, dd) << std::endl;
    std::cout << std::endl;

    gsl_vector_free(m);

    return VecPair(f, p);
}

static double errorDistance(const gsl_vector *a, const gsl_vector *b) {
    static_vector(diff, length);

    gsl_vector_memcpy(diff, a);
    gsl_vector_sub(diff, b);

    double dist(0.);

    // inf-norm
    //dist = abs(gsl_vector_get(diff, gsl_blas_idamax(diff)));
    // 2-norm
    dist = gsl_blas_dnrm2(diff);
    // 1-norm
    //dist = gsl_blas_dasum(diff);

    return dist;
}

static void computeMatA(gsl_matrix *A, const gsl_matrix *yT, ListCmu& C) {   
    for (size_t mu = 0; mu < length; ++mu) {
        auto row = gsl_matrix_submatrix(A, mu, 0, 1, length);

        // A[mu,:] = y' C_mu
        gsl_blas_dgemm(
                CblasNoTrans, CblasNoTrans,
                1., yT, C[mu].get(),
                0., &row.matrix
        );
    }
}

static void solveForX(gsl_permutation *perm, const gsl_matrix *A, gsl_vector *xHat, const gsl_matrix *L, const gsl_vector *dd, const double alpha) {
    // === LHS ===
    static_matrix(lhs, length, length);

    // lhs = a L'L
    gsl_blas_dgemm(CblasTrans, CblasNoTrans, alpha, L, L, 0., lhs);
    
    // lhs = A'A + a L'L
    gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1., A, A, 1., lhs);
    
    // === RHS === 
    static_vector(rhs, length);

    // rhs = A'dd
    gsl_blas_dgemv(CblasTrans, 1., A, dd, 0., rhs);

    // === Solve Pivoted Cholesky === 
    gsl_linalg_pcholesky_decomp(lhs, perm);
    
    gsl_linalg_pcholesky_solve(lhs, perm, rhs, xHat);
}

static void computeMatB(gsl_matrix *B, const gsl_vector *x, ListCmu& C) {
    for (size_t mu = 0; mu < length; ++mu) {
        auto row = gsl_matrix_row(B, mu);

        // B[mu,:] = C_mu x
        gsl_blas_dgemv(
                CblasNoTrans,
                1., C[mu].get(), x,
                0., &row.vector
        );
    }
}

static void solveForY(gsl_permutation *perm, const gsl_matrix* B, gsl_vector *yHat, const gsl_vector *ye, const gsl_vector *dd, const double beta, const double tau) {
    // === LHS ===
    static_matrix(lhs, length, length);
    
    // lhs = I
    gsl_matrix_set_identity(lhs);

    // lhs = B'B + (b + t) I
    gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1., B, B, beta + tau, lhs);
    
    // === RHS === 
    static_vector(rhs, length);

    // rhs = B'dd
    gsl_blas_dgemv(CblasTrans, 1., B, dd, 0., rhs);
    
    // rhs = B'dd + t ye
    gsl_blas_daxpy(tau, ye, rhs);

    // === Solve Pivoted Cholesky === 
    gsl_linalg_pcholesky_decomp(lhs, perm);
    
    gsl_linalg_pcholesky_solve(lhs, perm, rhs, yHat);
}

static void computeD(const gsl_matrix *A, const gsl_matrix *B, const gsl_vector *x, const gsl_vector *y, gsl_vector *d, double& dist) {

    static_vector(dx, length);
    static_vector(dy, length);

    // Ax = d
//    gsl_blas_dgemv(CblasNoTrans, 1., A, x, 0., dx);

    // By = d
    gsl_blas_dgemv(CblasNoTrans, 1., B, y, 0., d);
    
    /*// Average the two estimates
    gsl_vector_memcpy(d, dx);
    gsl_vector_add(d, dy);
    gsl_vector_scale(d, .5);

    // Get the error between the two
    dist = errorDistance(dx, dy);*/
}
