#include "lpc.h"


gsl_vector *lpcCoeffs(gsl_vector *x, size_t order) {
    gsl_matrix *X, *cov;
    gsl_vector *a, *b;
    size_t m, p;
    double chisq;

    X = gsl_matrix_calloc(order + 1, order + 1);
    cov = gsl_matrix_alloc(order + 1, order + 1);
    a = gsl_vector_alloc(order + 1);
    b = gsl_vector_calloc(order + 1);

    gsl_vector_set(b, 0, 1);

    for (m = 0; m <= order; ++m) {
        double xm = gsl_vector_get(x, m); 
        for (p = 0; p <= order && m + p <= order; ++p) {
            gsl_matrix_set(X, m + p, p, xm);
        }
    }

    auto work = gsl_multifit_linear_alloc(x->size, order + 1);

    gsl_multifit_linear(X, b, a, cov, &chisq, work);

    gsl_multifit_linear_free(work);
    gsl_matrix_free(X);
    gsl_vector_free(b);
    gsl_matrix_free(cov);

    return a;
}
