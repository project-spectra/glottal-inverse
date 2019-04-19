#include <gsl/gsl_blas.h>
#include "normalize.h"

void normalize(gsl_vector *x, double a) {

    size_t i = gsl_blas_idamax(x);
    double max = abs(gsl_vector_get(x, i));

    if (max >= 1e-10) {
        gsl_vector_scale(x, a / max);
    }
}


