#include "lpc.h"
#include <cstring>


double lpcCoeffs(double *lpc, gsl_vector *x, size_t order) {
    const int n = x->size;
    const int m = static_cast<int>(order + 1);

    double aut[m + 1];

    double err, eps;
    int i, j;

    j = m + 1;
    while (j--) {
        double d = 0.;
        for (i = j; i < n; ++i) {
            d += gsl_vector_get(x, i) * gsl_vector_get(x, i - j);
        }
        aut[j] = d;
    }

    err = aut[0] * (1. + 1e-10);
    eps = 1e-9 * aut[0] + 1e-10;

    for (i = 0; i < m; ++i) { 
        double r = -aut[i + 1];

        if (err < eps) {
            for (j = i; j < m; ++j) {
                lpc[j] = 0.;
            }
            goto done;
        }
        
        for (j = 0; j < i; ++j) {
            r -= lpc[j] * aut[i - j];
        }
        r /= err;

        lpc[i] = r;
        for (j = 0; j < i / 2; ++j) {
            double tmp = lpc[j];

            lpc[j] += r * lpc[i - 1 - j];
            lpc[i - 1 - j] += r * tmp;
        }

        if (i & 1) {
            lpc[j] += lpc[j] * r;
        };

        err *= 1. - r * r;
    }

done:

    double g = .99;
    double damp = g;
    for (j = 0; j < m; ++j) {
        lpc[j] *= damp;
        damp *= g;
    }

    return err;
}

/*
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
*/
