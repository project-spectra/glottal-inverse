#include "linalg.h"

#include <iostream>


gsl_vector *coords(gsl_vector *f) {
    size_t length, index;
    gsl_vector *U;
    double data;

    length = basisLength();

    U = gsl_vector_alloc(length);

    for (index = 0; index < length; ++index) {
        data = projection(f, basis(index));
        gsl_vector_set(U, index, data);
    }

    return U;
}

gsl_vector *uncoords(gsl_vector *u) {
    constexpr size_t length = basisLength();
    size_t k;

    gsl_vector *f, *g;
    
    f = gsl_vector_alloc(WINDOW_LENGTH * SAMPLE_RATE);
    g = gsl_vector_alloc(f->size);

    for (k = 0; k < length; ++k) {
        gsl_vector_set_zero(g);

        gsl_vector_memcpy(g, basis(k));
        gsl_vector_scale(g, gsl_vector_get(u, k));
    
        gsl_vector_add(f, g);
    }

    gsl_vector_free(g);

    return f;
}
