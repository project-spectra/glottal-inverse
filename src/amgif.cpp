#include "amgif.h"
#include "operators.h"
#include "constants.h"
#include "linalg.h"
#include "gsl_util.h"
#include "timer.h"


static void computeMatA(gsl_matrix *A, const gsl_vector *y, OpBuf& C);
static void computeMatB(gsl_matrix *B, const gsl_vector *x, OpBuf& C);

static void solveForX(const gsl_matrix *A, gsl_vector *xHat, const gsl_matrix *LL, const gsl_vector *dd, double alpha);
static void solveForY(const gsl_matrix* B, gsl_vector *yHat, const gsl_vector *ye, const gsl_vector *dd, double beta, double tau);

static void solveAxb(gsl_matrix *A, gsl_vector *u, const gsl_vector *f);

static void computeD(const gsl_matrix *A, const gsl_matrix *B, const gsl_vector *x, const gsl_vector *y, gsl_vector *d);

static double errorDistance(const gsl_vector *a, const gsl_vector *b);


VecTriplet computeAMGIF(
        OpBuf& Cbuf,
        gsl_vector *md, // signal
        gsl_vector *pe, // charac
        const gsl_matrix *L,
        const double alpha,
        const double beta,
        const double tau,
        const double eps
) {
    // Results: x, y, d
    static_vector(x);
    static_vector(y);
    static_vector(d);
   
    // dd = F(md)
    static_vector(dd);
    projForward(md, dd);

    // ye = F(pe)
    static_vector(ye);
    projForward(pe, ye);

    // yHat = ye
    static_vector(yHat);
    gsl_vector_memcpy(yHat, ye);

    // xHat = 0
    static_vector(xHat);
    gsl_vector_set_zero(xHat);

    // A, B
    static_matrix(A);
    static_matrix(B);

    // Calculate L'L firsthand.
    static_matrix(LL);
    gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1., L, L, 0., LL);
    
    double errSignal;
    size_t iters = 1;

    do {
        TIMER_START;

        gsl_vector_memcpy(y, yHat);  // y = yHat
        computeMatA(A, y, Cbuf);
       
        // TODO: find alpha parameter
        solveForX(A, xHat, LL, dd, alpha);  // estimated xHat

        gsl_vector_memcpy(x, xHat);  // x = xHat
        computeMatB(B, x, Cbuf);

        // TODO: find beta parameter
        solveForY(B, yHat, ye, dd, beta, tau);  // estimated yHat

        // Test for convergence:  Ax = By = d ~ dd
        computeD(A, B, x, y, d);
        
        errSignal = errorDistance(d, dd);

        TIMER_END("Iter");

        std::cerr << "Iteration " << iters << " with error " << errSignal << std::endl;

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

static void computeMatA(gsl_matrix *A, const gsl_vector *y, OpBuf& C) {
    TIMER_START;

    for (size_t mu = 0; mu < basisLength; ++mu) {
        // A[mu,:] = y' C_mu = (C_mu' y)'
        auto row = gsl_matrix_row(A, mu);

        gsl_blas_dgemv(CblasTrans, 1., C.get(mu), y, 0., &row.vector);
    }
    
    TIMER_END(" Compute A");
}

static void computeMatB(gsl_matrix *B, const gsl_vector *x, OpBuf& C) {
    TIMER_START;

    for (size_t mu = 0; mu < basisLength; ++mu) {
        // B[mu,:] = C_mu x
        auto row = gsl_matrix_row(B, mu);
        
        gsl_blas_dgemv(CblasNoTrans, 1., C.get(mu), x, 0., &row.vector);
    }

    TIMER_END(" Compute B");
}

static void solveForX(const gsl_matrix *A, gsl_vector *xHat, const gsl_matrix *LL, const gsl_vector *dd, const double alpha) {

    TIMER_START;

    // * LHS = A'A + a L'L
   
    static_matrix(lhs);
    gsl_matrix_memcpy(lhs, LL);
    
    gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1., A, A, alpha, lhs);

    // * RHS = A'dd
    
    static_vector(rhs);

    gsl_blas_dgemv(CblasTrans, 1., A, dd, 0., rhs);

    // === Solve Pivoted Cholesky ===

    solveAxb(lhs, xHat, rhs);
    
    TIMER_END(" Solve for X");
}

static void solveForY(const gsl_matrix* B, gsl_vector *yHat, const gsl_vector *ye, const gsl_vector *dd, const double beta, const double tau) {

    TIMER_START;

    // * LHS = B'B + (b + t) I

    static_matrix(lhs);
    gsl_matrix_set_identity(lhs);

    gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1., B, B, beta + tau, lhs);

    // * RHS = B'dd + t ye
    
    static_vector(rhs);

    gsl_blas_dgemv(CblasTrans, 1., B, dd, 0., rhs);
    gsl_blas_daxpy(tau, ye, rhs);

    // === Solve Pivoted Cholesky ===

    solveAxb(lhs, yHat, rhs);
    
    TIMER_END(" Solve for Y");
}

static void solveAxb(gsl_matrix *A, gsl_vector *x, const gsl_vector *b) { 
    static auto p = shared_ptr<gsl_permutation>(
            gsl_permutation_alloc(basisLength),
            gsl_permutation_free
    );

    TIMER_START;

    gsl_linalg_pcholesky_decomp(A, p.get());
    gsl_linalg_pcholesky_solve(A, p.get(), b, x);

    TIMER_END("  Pivoted Cholesky")

}

static void computeD(const gsl_matrix *A, const gsl_matrix *B, const gsl_vector *x, const gsl_vector *y, gsl_vector *d) {

    static_vector(dx);
    static_vector(dy);

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
    static_vector(diff);

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


