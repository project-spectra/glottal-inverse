#include "linalg.h"

#include <iostream>

double projection(gsl_vector *f, gsl_vector *g) {
    gsl_vector *u;

    u = gsl_vector_alloc(f->size);
    gsl_vector_memcpy(u, f);
    gsl_vector_mul(u, g);

    return integrate(u);
}
