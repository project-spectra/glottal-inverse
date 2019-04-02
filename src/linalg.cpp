#include "linalg.h"
#include <iostream>
#include <memory>
#include <cstring>

static constexpr size_t length = basis_length();

static auto haar = shared_ptr<gsl_wavelet>(
        gsl_wavelet_alloc(gsl_wavelet_haar, 2),
        gsl_wavelet_free
);

static auto workspace = shared_ptr<gsl_wavelet_workspace>(
        gsl_wavelet_workspace_alloc(length),
        gsl_wavelet_workspace_free
);


static void transform(double x[], double y[], gsl_wavelet_direction dir) {
    memcpy(y, x, length * sizeof(double));
    gsl_wavelet_transform(haar.get(), y, 1, length, dir, workspace.get());
}

void coords(double f[], double u[]) {
    return transform(f, u, gsl_wavelet_forward);
}

void uncoords(double u[], double f[]) {
    return transform(u, f, gsl_wavelet_backward);
}

void basis(gsl_vector *u, size_t index) {
    gsl_vector_set_basis(u, index);
}

void convolute(gsl_vector *f, gsl_vector *g, gsl_vector *w) {
    static double u[length];
    
    coords(f->data, u);

    for (size_t i = 0; i < length; ++i) {
        u[i] *= gsl_vector_get(g, i);
    }

    uncoords(u, w->data);
}

void normalize(gsl_vector *f) {
    size_t max_index = gsl_blas_idamax(f);
    double max = gsl_vector_get(f, max_index);

    // don't normalize all the way
    gsl_vector_scale(f, .99 / abs(max));
}
