#include "linalg.h"
#include <iostream>


gsl_vector *convolute(gsl_vector *f, gsl_vector *g) {
    const size_t N = f->size;

    gsl_vector *conv, *u;
    size_t k, s;
    double data;

    conv = gsl_vector_alloc(N);
    u = gsl_vector_alloc(N);

    for (k = 0; k < N; ++k) {
        for (s = 0; s < N; ++s) {
            if (k-s >= 0 && k-s < N) {
                data = gsl_vector_get(f, k - s);
            } else {
                data = 0.0;
            }

            gsl_vector_set(u, s, data);
        }
        
        gsl_vector_mul(u, g);

        gsl_vector_set(conv, k, integrate(u));
    }

    return conv;
}

