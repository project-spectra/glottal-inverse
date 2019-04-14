#include "linalg.h"


void normalize(gsl_vector *x) {

    size_t i = gsl_blas_idamax(x);
    double max = abs(gsl_vector_get(x, i));

    gsl_vector_scale(x, .95 / max);
    
}
