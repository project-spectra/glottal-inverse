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
        gsl_wavelet_workspace_alloc(basis_length()),
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
    static double v[length];
    
    coords(f->data, u);
    coords(g->data, v);

    for (size_t i = 0; i < length; ++i) {
        u[i] *= v[i];
    }

    uncoords(u, w->data);
}

