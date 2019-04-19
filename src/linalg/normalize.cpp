#include "linalg.h"

#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

void normalize(gsl_vector *x, double a) {

    size_t i = gsl_blas_idamax(x);
    double max = abs(gsl_vector_get(x, i));

    if (max >= 1e-10) {
        gsl_vector_scale(x, a / max);
    }
}

void normalize_complex(gsl_vector_complex *x, double a) {

    size_t i = gsl_blas_izamax(x);
    double max = gsl_complex_abs(gsl_vector_complex_get(x, i));

    if (max >= 1e-10) {
        gsl_vector_complex_scale(x, gsl_complex_rect(a / max, 0.));
    }
}
