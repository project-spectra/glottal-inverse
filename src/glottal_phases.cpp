#include "glottal_phases.h"
#include "linalg.h"
#include "gsl_util.h"

#include <iostream>



void estimatePhases(gsl_vector *g, double T0, double *Oq, double *Cq)
{
    const size_t N = g->size;

    size_t n;
    double data;
    
    // apply Teager energy operator
    auto x = gsl_vector_alloc(N);
    
    for (n = 0; n < N; ++n) {
        data = gsl_vector_get(g, n);
        data *= data;
        data -= ((n >= 1) ? gsl_vector_get(g, n - 1) : 1.) *
                ((n < N - 1) ? gsl_vector_get(g, n + 1) : 1.);

        gsl_vector_set(x, n, data);
    }
    gsl_vector_mul(x, g);
    normalize(x);

    // turn into FT
    auto y = gsl_vector_complex_alloc(2*N);
    
    for (n = 0; n < N; ++n) {
        gsl_vector_complex_set(y, n, gsl_complex_rect(gsl_vector_get(x, n), 0.));
    }
    for (n = N; n < 2 * N; ++n) {
        gsl_vector_complex_set(y, n, gsl_complex_rect(gsl_vector_get(x, 2 * N - 1 - n), 0.));
    }
    
    // inverse FT for even signal
    gsl_fft_complex_radix2_inverse(y->data, y->stride, y->size);

    gsl_vector_complex_scale(y, gsl_complex_rect(1.06, 0.));
    
    gsl_fft_complex_radix2_forward(y->data, y->stride, y->size);

    double delta((2. * M_PI * SAMPLE_RATE) / (2. * N));
    double r;

    for (n = 0; n < N; ++n) {
        auto yn = gsl_vector_complex_get(y, 2 * N - 1 - n);
        r = gsl_complex_abs(yn);

        if (r == 1.) {
            data = M_PI_2;
        } else if (r > 1.) {
            data = 2. * (atan((r * delta) / (1. - r)) + M_PI);
        } else {
            data = 2. * (atan((r * delta) / (1. - r)));
        }
        
        gsl_vector_set(x, n, data);
    }

    gsl_vector_memcpy(g, x);

    gsl_vector_free(x);
    gsl_vector_complex_free(y);
}

