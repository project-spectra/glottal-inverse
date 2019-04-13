#include "linalg.h"


static auto haar = shared_ptr<gsl_wavelet>(
        gsl_wavelet_alloc(gsl_wavelet_haar, 2),
        gsl_wavelet_free
);

static auto workspace = shared_ptr<gsl_wavelet_workspace>(
        gsl_wavelet_workspace_alloc(basisLength),
        gsl_wavelet_workspace_free
);

static void transform(gsl_vector *y, gsl_wavelet_direction dir) {
    gsl_wavelet_transform(haar.get(), y->data, 1, basisLength, dir, workspace.get());
}

void projForward(gsl_vector *f) {
    transform(f, gsl_wavelet_forward);
}

void projForward(gsl_vector *f, gsl_vector *u) {
    gsl_vector_memcpy(u, f);
    transform(u, gsl_wavelet_forward);
}

void projBackward(gsl_vector *u) {
    transform(u, gsl_wavelet_backward);
}

void projBackward(gsl_vector *u, gsl_vector *f) {
    gsl_vector_memcpy(f, u);
    transform(f, gsl_wavelet_backward);
}




