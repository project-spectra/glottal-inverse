#include "amgif.h"
#include "constants.h"
#include "linalg.h"

#include <chrono>
#include <iostream>


using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;

using ListCmu = const vector<mat_operator>;
using VecPair = pair<gsl_vector *, gsl_vector *>;

using VecArr = array<double, length>;

// macros to define static (and allocated on stack)
// vectors and matrices to avoid reallocation
// whenever the function is called.

#define static_vector(name, length)  \
    static VecArr arr_##name;  \
    static auto view_##name = gsl_vector_view_array(arr_##name.data(), length);  \
    static auto name = &view_##name.vector;  \


static gsl_spmatrix *computeTransMat(const gsl_spmatrix *A);

static gsl_spmatrix *makeMatrixForX(const gsl_vector *x);
static gsl_spmatrix *makeMatrixForY(const gsl_vector *y);

static void computeMatA(gsl_spmatrix *A, const gsl_spmatrix *yMat, const ListCmu& C);
static void computeMatB(gsl_spmatrix *B, const gsl_spmatrix *xMat, const ListCmu& C);

static void solveForX(gsl_permutation *perm, const gsl_spmatrix *A, gsl_vector *xHat, const gsl_spmatrix *LL, const gsl_vector *dd, double alpha);
static void solveForY(gsl_permutation *perm, const gsl_spmatrix* B, gsl_vector *yHat, const gsl_vector *ye, const gsl_vector *dd, double beta, double tau);


static void computeD(const gsl_spmatrix *A, const gsl_spmatrix *B, const gsl_vector *x, const gsl_vector *y, gsl_vector *d);

static double errorDistance(const gsl_vector *a, const gsl_vector *b);


VecTriplet computeAMGIF(
        ListCmu& C,
        gsl_vector *md, // signal
        gsl_vector *pe, // charac
        const mat_operator& L,
        const double alpha,
        const double beta,
        const double tau,
        const double eps
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
    coords(md->data, arr_dd.data());

    // ye = F(pe)
    static_vector(ye, length);
    coords(pe->data, arr_ye.data());

    // yHat = ye
    static_vector(yHat, length);
    gsl_vector_memcpy(yHat, ye);

    static_vector(xHat, length);

    // A, B
    auto A = gsl_spmatrix_alloc_nzmax(length, length, (length * length) / 2, GSL_SPMATRIX_CCS);
    
    auto B = gsl_spmatrix_alloc_nzmax(length, length, (length * length) / 2, GSL_SPMATRIX_CCS);

    // Calculate L'L firsthand.
    auto LL = computeTransMat(L.get());

    // Make the x and y matrices for easier computation.
    auto xMat = makeMatrixForX(x);
    auto yMat = makeMatrixForY(y);

    double errSignal;
    size_t iters = 1;

    do {
        gsl_vector_memcpy(y, yHat);  // y = yHat
        computeMatA(A, yMat, C);
        
        // TODO: find alpha parameter
        solveForX(perm.get(), A, xHat, LL, dd, alpha);  // estimated xHat

        gsl_vector_memcpy(x, xHat);  // x = xHat
        computeMatB(B, xMat, C);

        // TODO: find beta parameter
        solveForY(perm.get(), B, yHat, ye, dd, beta, tau);  // estimated yHat

        // Test for convergence:  Ax = By = d ~ dd
        computeD(A, B, x, y, d);
        
        errSignal = errorDistance(d, dd);
        
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

    return VecTriplet(m, f, p);
}

static gsl_spmatrix *makeMatrixForX(const gsl_vector *x) {

    auto xDense = gsl_matrix_alloc(length, length);

    // one Y on each row
    for (size_t j = 0; j < length; ++j) {
        gsl_matrix_set_row(xDense, j, x);
    }

    auto xTriplet = gsl_spmatrix_alloc(length, length);
    gsl_spmatrix_d2sp(xTriplet, xDense);

    auto xMat = gsl_spmatrix_ccs(xTriplet);

    gsl_matrix_free(xDense);
    gsl_spmatrix_free(xTriplet);

    return xMat;
}

static gsl_spmatrix *makeMatrixForY(const gsl_vector *y) {
    auto yDense = gsl_matrix_alloc(length, length);

    // one X on each column
    for (size_t i = 0; i < length; ++i) {
        gsl_matrix_set_col(yDense, i, y);
    }

    auto yTriplet = gsl_spmatrix_alloc(length, length);
    gsl_spmatrix_d2sp(yTriplet, yDense);

    auto yMat = gsl_spmatrix_ccs(yTriplet);

    gsl_matrix_free(yDense);
    gsl_spmatrix_free(yTriplet);

    return yMat;
}

static gsl_spmatrix *computeTransMat(const gsl_spmatrix *A) {
  size_t nzmax = gsl_spmatrix_nnz(A);

  // transpose A into At
  auto At = gsl_spmatrix_alloc_nzmax(length, length, nzmax, GSL_SPMATRIX_CCS);
  gsl_spmatrix_transpose_memcpy(At, A);

  // do At \dot A
  auto ATA = gsl_spmatrix_alloc_nzmax(length, length, nzmax, GSL_SPMATRIX_CCS);
  gsl_spblas_dgemm(1., At, A, ATA);

  gsl_spmatrix_free(At);

  return ATA;
}

static void computeMatA(gsl_spmatrix *A, const gsl_spmatrix *yMat, const ListCmu& C) {
    for (size_t mu = 0; mu < length; ++mu) {
        // A[mu,:] = y' C_mu
        gsl_spblas_dgemm(1., yMat, C[mu].get(), A);
    }
}

static void computeMatB(gsl_spmatrix *B, const gsl_spmatrix *xMat, const ListCmu& C) {
    for (size_t mu = 0; mu < length; ++mu) {
        // B[mu,:] = C_mu x
        gsl_spblas_dgemm(1., C[mu].get(), xMat, B);
    }
}

static void solveForX(gsl_permutation *perm, const gsl_spmatrix *A, gsl_vector *xHat, const gsl_spmatrix *LL, const gsl_vector *dd, const double alpha) {

    // * LHS = A'A + a L'L

    auto scaledLL = gsl_spmatrix_alloc_nzmax(length, length, gsl_spmatrix_nnz(LL), GSL_SPMATRIX_CCS);
    gsl_spmatrix_scale(scaledLL, alpha);

    auto AtA = computeTransMat(A);
    
    auto sparseLhs = gsl_spmatrix_alloc(length, length);
    gsl_spmatrix_add(sparseLhs, AtA, scaledLL);

    // * RHS = A'dd
    
    static_vector(rhs, length);

    gsl_spblas_dgemv(CblasTrans, 1., A, dd, 0., rhs);

    // === Solve Pivoted Cholesky ===

    auto lhs = gsl_matrix_alloc(length, length);

    gsl_spmatrix_sp2d(lhs, sparseLhs);

    gsl_linalg_pcholesky_decomp(lhs, perm);  
    gsl_linalg_pcholesky_solve(lhs, perm, rhs, xHat);
   
    // free stuff
    gsl_spmatrix_free(scaledLL);
    gsl_spmatrix_free(AtA);
    gsl_spmatrix_free(sparseLhs);
    gsl_matrix_free(lhs);
}

static void solveForY(gsl_permutation *perm, const gsl_spmatrix* B, gsl_vector *yHat, const gsl_vector *ye, const gsl_vector *dd, const double beta, const double tau) {

    // * LHS = B'B + (b + t) I

    auto scaledId = gsl_spmatrix_alloc_nzmax(length, length, (length - 1) * length, GSL_SPMATRIX_TRIPLET);
    for (size_t k = 0; k < length; ++k) {
      gsl_spmatrix_set(scaledId, k, k, (beta + tau));
    }

    auto BtB = computeTransMat(B);

    auto sparseLhs = gsl_spmatrix_alloc(length, length);
    gsl_spmatrix_add(sparseLhs, BtB, scaledId);

    // * RHS = B'dd + t ye
    
    static_vector(rhs, length);

    gsl_spblas_dgemv(CblasTrans, 1., B, dd, 0., rhs);
    gsl_blas_daxpy(tau, ye, rhs);

    // === Solve Pivoted Cholesky ===

    auto lhs = gsl_matrix_alloc(length, length);

    gsl_spmatrix_sp2d(lhs, sparseLhs);

    gsl_linalg_pcholesky_decomp(lhs, perm);  
    gsl_linalg_pcholesky_solve(lhs, perm, rhs, yHat);
   
    // free stuff
    gsl_spmatrix_free(scaledId);
    gsl_spmatrix_free(BtB);
    gsl_spmatrix_free(sparseLhs);
    gsl_matrix_free(lhs);
}

static void computeD(const gsl_spmatrix *A, const gsl_spmatrix *B, const gsl_vector *x, const gsl_vector *y, gsl_vector *d) {

    static_vector(dx, length);
    static_vector(dy, length);

    // Ax = d
    gsl_spblas_dgemv(CblasNoTrans, 1., A, x, 0., dx);

    // By = d
    gsl_spblas_dgemv(CblasNoTrans, 1., B, y, 0., dy);
    
    // Average the two estimates
    gsl_vector_memcpy(d, dx);
    gsl_vector_add(d, dy);
    gsl_vector_scale(d, .5);
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


