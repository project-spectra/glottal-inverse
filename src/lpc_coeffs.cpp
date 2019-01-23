#include "lpc.h"


gsl_vector *lpcCoeffs(gsl_vector *x, size_t M, size_t order) {
    gsl_matrix *X;
    gsl_vector *a, *b;
    size_t m, p;

    X = gsl_matrix_calloc(M + order + 1, order + 1);
    a = gsl_vector_alloc(order + 1);
    b = gsl_vector_calloc(order + 1);

    gsl_vector_set(b, 0, 1);

    for (m = 0; m < M; ++m) {
        double xm = gsl_vector_get(x, m); 
        for (p = 0; p <= order; ++p) {
            gsl_matrix_set(X, m + p, p, xm);
        }
    }

    auto work = gsl_multifit_linear_alloc(M, order + 1);

    gsl_multifit_linear(X, b, a, nullptr, nullptr, work);

    gsl_multifit_linear_free(work);
    gsl_matrix_free(X);
    gsl_vector_free(b);

    return a;
}
