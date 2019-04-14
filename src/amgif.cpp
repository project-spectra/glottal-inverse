#include "amgif.h"
#include "operators.h"
#include "constants.h"
#include "linalg.h"

#include <chrono>
#include <iostream>


using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;


// macros to define static (and allocated on stack)
// vectors and matrices to avoid reallocation
// whenever the function is called.

#define static_vector(name, length)  \
    static VecArr arr_##name;  \
    static auto view_##name = gsl_vector_view_array(arr_##name.data(), length);  \
    static auto name = &view_##name.vector;  \


using OpBuf = OperatorBuffer;


static gsl_matrix *computeTransMat(const gsl_matrix *A);

static gsl_matrix *computeMatA(const gsl_vector *y, OpBuf& C);
static gsl_matrix *computeMatB(const gsl_vector *x, OpBuf& C);

static void solveForX(const gsl_matrix *A, gsl_vector *xHat, const gsl_matrix *LL, const gsl_vector *dd, double alpha);
static void solveForY(const gsl_matrix* B, gsl_vector *yHat, const gsl_vector *ye, const gsl_vector *dd, double beta, double tau);

static void solveAxb(gsl_matrix *A, gsl_vector *u, const gsl_vector *f);

static void computeD(const gsl_matrix *A, const gsl_matrix *B, const gsl_vector *x, const gsl_vector *y, gsl_vector *d);

static double errorDistance(const gsl_vector *a, const gsl_vector *b);


VecTriplet computeAMGIF(
        gsl_vector *md, // signal
        gsl_vector *pe, // charac
        const gsl_matrix *L,
        const double alpha,
        const double beta,
        const double tau,
        const double eps
) {
    OpBuf Cbuf(MAX_CAPACITY);

    // Results: x, y, d
    static_vector(x, basisLength);
    static_vector(y, basisLength);
    static_vector(d, basisLength);
   
    // dd = F(md)
    static_vector(dd, basisLength);
    projForward(md, dd);

    // ye = F(pe)
    static_vector(ye, basisLength);
    projForward(pe, ye);

    // yHat = ye
    static_vector(yHat, basisLength);
    gsl_vector_memcpy(yHat, ye);

    // xHat = 0
    static_vector(xHat, basisLength);
    gsl_vector_set_zero(xHat);

    // Calculate L'L firsthand.
    auto LL = computeTransMat(L);

    double errSignal;
    size_t iters = 1;

    do {
        gsl_vector_memcpy(y, yHat);  // y = yHat
        auto A = computeMatA(y, Cbuf);
        
        // TODO: find alpha parameter
        solveForX(A, xHat, LL, dd, alpha);  // estimated xHat

        gsl_vector_memcpy(x, xHat);  // x = xHat
        auto B = computeMatB(x, Cbuf);

        // TODO: find beta parameter
        solveForY(B, yHat, ye, dd, beta, tau);  // estimated yHat

        // Test for convergence:  Ax = By = d ~ dd
        computeD(A, B, x, y, d);
        
        errSignal = errorDistance(d, dd);
       
        gsl_matrix_free(A);
        gsl_matrix_free(B);

        ++iters;
    } while (errSignal > eps && iters <= MAX_ITER);

    // d = F(m) : wavelet coordinates for the speech function
    // x = F(f) : wavelet coordinates for the input function
    // y = F(p) : wavelet coordinates for the caracteristic function
    
    auto m = gsl_vector_alloc(basisLength);
    auto f = gsl_vector_alloc(basisLength);
    auto p = gsl_vector_alloc(basisLength);

    projBackward(d, m);
    projBackward(x, f);
    projBackward(y, p);

    return VecTriplet(m, f, p);
}

static gsl_matrix *computeTransMat(const gsl_matrix *A) {
    gsl_matrix *ATA = gsl_matrix_alloc(basisLength, basisLength);

    gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1., A, A, 0., ATA);
    
    return ATA;
}

static gsl_matrix *computeMatA(const gsl_vector *y, OpBuf& C) {
    static_vector(res, basisLength);

    auto A = gsl_matrix_alloc(basisLength, basisLength);

    for (size_t mu = 0; mu < basisLength; ++mu) {
        // A[mu,:] = y' C_mu = (C_mu' y)'
        gsl_blas_dgemv(CblasTrans, 1., C.get(mu), y, 0., res);
        gsl_matrix_set_row(A, mu, res);
    }

    return A;
}

static gsl_matrix *computeMatB(const gsl_vector *x, OpBuf& C) {
    static_vector(res, basisLength);

    auto B = gsl_matrix_alloc(basisLength, basisLength);

    for (size_t mu = 0; mu < basisLength; ++mu) {
        // B[mu,:] = C_mu x
        gsl_blas_dgemv(CblasNoTrans, 1., C.get(mu), x, 0., res);
        gsl_matrix_set_row(B, mu, res);
    }

    return B;
}

static void solveForX(const gsl_matrix *A, gsl_vector *xHat, const gsl_matrix *LL, const gsl_vector *dd, const double alpha) {

    // * LHS = A'A + a L'L
    
    auto lhs = gsl_matrix_alloc(basisLength, basisLength);
    gsl_matrix_memcpy(lhs, LL);
    
    gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1., A, A, alpha, lhs);

    // * RHS = A'dd
    
    static_vector(rhs, basisLength);

    gsl_blas_dgemv(CblasTrans, 1., A, dd, 0., rhs);

    // === Solve Pivoted Cholesky ===

    solveAxb(lhs, xHat, rhs);

    // free stuff
    gsl_matrix_free(lhs);
}

static void solveForY(const gsl_matrix* B, gsl_vector *yHat, const gsl_vector *ye, const gsl_vector *dd, const double beta, const double tau) {

    // * LHS = B'B + (b + t) I

    auto lhs = gsl_matrix_alloc(basisLength, basisLength);
    gsl_matrix_set_identity(lhs);

    gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1., B, B, beta + tau, lhs);

    // * RHS = B'dd + t ye
    
    static_vector(rhs, basisLength);

    gsl_blas_dgemv(CblasTrans, 1., B, dd, 0., rhs);
    gsl_blas_daxpy(tau, ye, rhs);

    // === Solve Pivoted Cholesky ===

    solveAxb(lhs, yHat, rhs);
   
    // free stuff
    gsl_matrix_free(lhs);
}

static void solveAxb(gsl_matrix *A, gsl_vector *x, const gsl_vector *b) {
    static auto p = shared_ptr<gsl_permutation>(
            gsl_permutation_alloc(basisLength),
            gsl_permutation_free
    );

    gsl_linalg_pcholesky_decomp(A, p.get());
    gsl_linalg_pcholesky_solve(A, p.get(), b, x);

}

static void computeD(const gsl_matrix *A, const gsl_matrix *B, const gsl_vector *x, const gsl_vector *y, gsl_vector *d) {

    static_vector(dx, basisLength);
    static_vector(dy, basisLength);

    // Ax = d
    gsl_blas_dgemv(CblasNoTrans, 1., A, x, 0., dx);

    // By = d
    gsl_blas_dgemv(CblasNoTrans, 1., B, y, 0., dy);
    
    // Average the two estimates
    gsl_vector_memcpy(d, dx);
    gsl_vector_add(d, dy);
    gsl_vector_scale(d, .5);
}

static double errorDistance(const gsl_vector *a, const gsl_vector *b) {
    static_vector(diff, basisLength);

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


