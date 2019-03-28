#include "amgif.h"
#include "linalg.h"

#include <iostream>
#include <chrono>


using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;


double deviationAMGIF(gsl_vector *y, gsl_vector *yi) {
    // 2-norm distance  
    gsl_vector *sub = gsl_vector_alloc(y->size);
    gsl_vector_memcpy(sub, y);
    gsl_vector_sub(sub, yi);
    
    double dist = gsl_blas_dnrm2(sub);

    gsl_vector_free(sub);

    return dist;
}

pair<gsl_vector *, gsl_vector*> computeAMGIF(
        vector<mat_operator>& C,
        gsl_function *me,
        gsl_function *pe,
        mat_operator& L,
        double alpha,
        double beta,
        double tau,
        double eps
) {

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

#define view_vector(name) (&view_##name.vector)
#define view_matrix(name) (&view_##name.matrix)

    static constexpr size_t length = basis_length();

    static_vector(d, length);
    static_vector(x, length);
    static_vector(y, length);
    static_vector(rhs, length);

    static_matrix(A, length, length);
    static_matrix(B, length, length);
    static_matrix(lhs, length, length);

    size_t mu;
    double err;

    std::cout << "coords" << std::endl;

    auto di = coords(me);
    auto yi = coords(pe);

    std::cout << "done coords" << std::endl;

    gsl_vector_memcpy(y, yi);

    auto div = gsl_matrix_view_vector(di, length, 1);
    auto dv = gsl_matrix_view_vector(d, length, 1);
    auto xv = gsl_matrix_view_vector(x, length, 1);
    auto yv = gsl_matrix_view_vector(y, length, 1);
    auto rhsv = gsl_matrix_view_vector(rhs, length, 1);

    size_t iter = 1;
    do {
        // Minimize for Y

        for (mu = 0; mu < length; ++mu) {
            auto row = gsl_matrix_submatrix(A, mu, 0, 1, length);
            
            gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, &yv.matrix, C[mu].get(), 0.0, &row.matrix);
        }

        std::cout << "solve for Y" << std::endl;

        gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, L.get(), L.get(), 0.0, lhs);
        gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, A, A, alpha, lhs);

        gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, A, &div.matrix, 0.0, &rhsv.matrix);

        gsl_linalg_cholesky_decomp(lhs);
        gsl_linalg_cholesky_solve(lhs, rhs, x);

        // Minimize for X

        for (mu = 0; mu < length; ++mu) {
            auto column = gsl_matrix_submatrix(B, 0, mu, length, 1);

            gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, C[mu].get(), &xv.matrix, 0.0, &column.matrix);
        }
        
        std::cout << "solve for X" << std::endl;

        gsl_matrix_set_identity(lhs);
        gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, B, B, beta + tau, lhs);

        gsl_vector_memcpy(rhs, yi);
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, B, &div.matrix, tau, &rhsv.matrix);

        gsl_linalg_cholesky_decomp(lhs);
        gsl_linalg_cholesky_solve(lhs, rhs, y);

        // Average the two for this estimation
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, A, &xv.matrix, 0.0, &dv.matrix);
        gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 0.5, B, &yv.matrix, 0.5, &dv.matrix);

        err = deviationAMGIF(x, di);

        std::cout << "  * Iteration " << iter << " with error " << err << std::endl;

        iter++;
    } while (err > eps && iter <= MAX_ITER);

    std::cout << "  * Converged in " << iter-1 << " iterations with final error " << err << std::endl;

    gsl_vector_free(di);
    gsl_vector_free(yi);

    return pair<gsl_vector *, gsl_vector *>(x, y);
}
