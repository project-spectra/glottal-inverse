#include "amgif.h"
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


static gsl_spmatrix *computeTransMat(const gsl_spmatrix *A);

static gsl_spmatrix *computeMatA(const gsl_vector *y, const ListCmu& C);
static gsl_spmatrix *computeMatB(const gsl_vector *x, const ListCmu& C);

static void solveForX(const gsl_spmatrix *A, gsl_vector *xHat, const gsl_spmatrix *LL, const gsl_vector *dd, double alpha, double eps);
static void solveForY(const gsl_spmatrix* B, gsl_vector *yHat, const gsl_vector *ye, const gsl_vector *dd, double beta, double tau, double eps);

static void solveAxbIter(const gsl_spmatrix *A, gsl_vector *u, const gsl_vector *f, const double eps);

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

    // xHat = 0
    static_vector(xHat, length);
    gsl_vector_set_zero(xHat);

    // Calculate L'L firsthand.
    auto LL = computeTransMat(L.get());

    double errSignal;
    size_t iters = 1;

    do {
        gsl_vector_memcpy(y, yHat);  // y = yHat
        auto A = computeMatA(y, C);
        
        // TODO: find alpha parameter
        solveForX(A, xHat, LL, dd, alpha, eps);  // estimated xHat

        gsl_vector_memcpy(x, xHat);  // x = xHat
        auto B = computeMatB(x, C);

        // TODO: find beta parameter
        solveForY(B, yHat, ye, dd, beta, tau, eps);  // estimated yHat

        // Test for convergence:  Ax = By = d ~ dd
        computeD(A, B, x, y, d);
        
        errSignal = errorDistance(d, dd);
       
        gsl_spmatrix_free(A);
        gsl_spmatrix_free(B);

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

static gsl_spmatrix *computeMatA(const gsl_vector *y, const ListCmu& C) {
    static_vector(res, length);

    auto resM = gsl_spmatrix_alloc(length, length);

    for (size_t mu = 0; mu < length; ++mu) {
        // A[mu,:] = y' C_mu = (C_mu' y)'
        gsl_spblas_dgemv(CblasTrans, 1., C[mu].get(), y, 0., res);
      
        for (size_t j = 0; j < length; ++j) {
            gsl_spmatrix_set(resM, mu, j, gsl_vector_get(res, j));
        }
    }

    return gsl_spmatrix_ccs(resM);
}

static gsl_spmatrix *computeMatB(const gsl_vector *x, const ListCmu& C) {
    static_vector(res, length);

    auto resM = gsl_spmatrix_alloc(length, length);

    for (size_t mu = 0; mu < length; ++mu) {
        // B[mu,:] = C_mu x
        gsl_spblas_dgemv(CblasNoTrans, 1., C[mu].get(), x, 0., res);

        for (size_t j = 0; j < length; ++j) {
            gsl_spmatrix_set(resM, mu, j, gsl_vector_get(res, j));
        }
    }

    return gsl_spmatrix_ccs(resM);
}

static void solveForX(const gsl_spmatrix *A, gsl_vector *xHat, const gsl_spmatrix *LL, const gsl_vector *dd, const double alpha, const double eps) {

    // * LHS = A'A + a L'L
    
    auto scaledLL = gsl_spmatrix_alloc_nzmax(length, length, gsl_spmatrix_nnz(LL), GSL_SPMATRIX_CCS);
    gsl_spmatrix_memcpy(scaledLL, LL);
    gsl_spmatrix_scale(scaledLL, alpha);

    auto AtA = computeTransMat(A);
    
    auto lhs = gsl_spmatrix_alloc_nzmax(length, length, GSL_MAX(gsl_spmatrix_nnz(scaledLL),
                                                                gsl_spmatrix_nnz(AtA)), GSL_SPMATRIX_CCS);
    gsl_spmatrix_add(lhs, AtA, scaledLL);

    // * RHS = A'dd
    
    static_vector(rhs, length);

    gsl_spblas_dgemv(CblasTrans, 1., A, dd, 0., rhs);

    // === Solve Pivoted Cholesky ===

    solveAxbIter(lhs, xHat, rhs, eps);
   
    // free stuff
    gsl_spmatrix_free(scaledLL);
    gsl_spmatrix_free(AtA);
    gsl_spmatrix_free(lhs);
}

static void solveForY(const gsl_spmatrix* B, gsl_vector *yHat, const gsl_vector *ye, const gsl_vector *dd, const double beta, const double tau, const double eps) {

    // * LHS = B'B + (b + t) I

    auto scaledIdTri = gsl_spmatrix_alloc_nzmax(length, length, length, GSL_SPMATRIX_TRIPLET);
    for (size_t k = 0; k < length; ++k) {
      gsl_spmatrix_set(scaledIdTri, k, k, beta + tau);
    }
    auto scaledId = gsl_spmatrix_ccs(scaledIdTri);
    gsl_spmatrix_free(scaledIdTri);

    auto BtB = computeTransMat(B);

    auto lhs = gsl_spmatrix_alloc_nzmax(length, length, GSL_MAX(gsl_spmatrix_nnz(scaledId),
                                                                gsl_spmatrix_nnz(BtB)), GSL_SPMATRIX_CCS); 
    gsl_spmatrix_add(lhs, BtB, scaledId);

    // * RHS = B'dd + t ye
    
    static_vector(rhs, length);

    gsl_spblas_dgemv(CblasTrans, 1., B, dd, 0., rhs);
    gsl_blas_daxpy(tau, ye, rhs);

    // === Solve Pivoted Cholesky ===

    solveAxbIter(lhs, yHat, rhs, eps);
   
    // free stuff
    gsl_spmatrix_free(scaledId);
    gsl_spmatrix_free(BtB);
    gsl_spmatrix_free(lhs);
}

static void solveAxbIter(const gsl_spmatrix *A, gsl_vector *u, const gsl_vector *f, const double eps) {
    const auto workT = gsl_splinalg_itersolve_gmres;
    auto work = gsl_splinalg_itersolve_alloc(workT, length, 0);

    size_t iter(1);  
    double residual;
    int status;

    /* solve the system A u = f */
    do {
        status = gsl_splinalg_itersolve_iterate(A, f, eps, u, work);

        /* print out residual norm ||A*u - f|| */
        //residual = gsl_splinalg_itersolve_normr(work);

        if (status == GSL_SUCCESS) {
          fprintf(stderr, "Converged\n");
        }
    } while (status == GSL_CONTINUE && ++iter <= MAX_SUB_ITER);

    gsl_splinalg_itersolve_free(work);
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


