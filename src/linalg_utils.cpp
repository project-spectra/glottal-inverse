#include "linalg.h"


void normalize(gsl_vector *x) {

    double max = gsl_vector_get(x, gsl_blas_idamax(x));
    gsl_vector_scale(x, 1. / abs(max));

}
