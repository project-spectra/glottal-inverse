#include "linalg.h"
#include <iostream>
#include <memory>

/* Haar wavelets and wavelet workspace */

static auto haar = shared_ptr<gsl_wavelet>(
        gsl_wavelet_alloc(gsl_wavelet_haar, 2),
        gsl_wavelet_free
);

static auto work = shared_ptr<gsl_wavelet_workspace>(
        gsl_wavelet_workspace_alloc(2 << J),
        gsl_wavelet_workspace_free
);

/* Functions */

static gsl_vector *transform(gsl_vector *x, gsl_wavelet_direction dir) {
    gsl_vector *y = gsl_vector_alloc(2 << J);
    gsl_vector_memcpy(y, x);
    gsl_wavelet_transform(haar.get(), y->data, 1, 2 << J, dir, work.get());
    return y;
}

gsl_vector *coords(gsl_vector *f) {
    return transform(f, gsl_wavelet_forward);
}

gsl_vector *uncoords(gsl_vector *u) {
    return transform(u, gsl_wavelet_backward);
}

gsl_vector *basis(size_t index) {
    gsl_vector *u = gsl_vector_alloc(2 << J);
    gsl_vector_set_basis(u, index);
    return u;
}

gsl_vector *convolute(gsl_vector *f, gsl_vector *g) {
    gsl_vector *u, *v, *w;
    
    u = coords(f);
    v = coords(g);

    gsl_vector_mul(u, v);

    w = uncoords(u);

    gsl_vector_free(u);
    gsl_vector_free(v);

    return w;
}
