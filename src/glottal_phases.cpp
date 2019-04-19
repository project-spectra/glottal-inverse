#include "glottal_phases.h"
#include "linalg.h"
#include "filter.h"
#include "derivative.h"
#include "gsl_util.h"

#include <iostream>



void estimatePhases(gsl_vector *g, double T0, double *Oq, double *Cq)
{
    const size_t N = g->size;

    size_t n;
    double data;
    
    // apply modified Teager energy operator
    auto x = gsl_vector_alloc(N);
    
    for (n = 0; n < N; ++n) {
        data = gsl_vector_get(g, n);
        data *= data;
        data -= ((n >= 1) ? gsl_vector_get(g, n - 1) : 1.) *
                ((n < N - 1) ? gsl_vector_get(g, n + 1) : 1.);

        gsl_vector_set(x, n, data);
    }
    gsl_vector_mul(x, g);

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

    // scale up and FT on the causal portion
    gsl_vector_complex_scale(y, gsl_complex_rect(1.06, 0.));

    gsl_fft_complex_radix2_forward(y->data, y->stride, y->size);

    // compute phase difference
    const double delta = (2. * M_PI * SAMPLE_RATE) / (2. * N);
    gsl_vector *theta = gsl_vector_alloc(N);
    
    for (n = 0; n < N; ++n) {
        auto yn = gsl_vector_complex_get(y, n);
        double r = gsl_complex_abs(yn);

        double y = r * sin(delta);
        double x = 1 - r * cos(delta);

        if (x > 0) {
            data = atan2(y, x);
        } else if (x == 0) {
            data = y > 0 ? M_PI_2 : -M_PI_2;
        } else if (y >= 0) {
            data = atan2(y, x) + M_PI;
        } else /*(y < 0 )*/ { 
            data = atan2(y, x) - M_PI;
        }

        gsl_vector_set(theta, n, 2 * data);
    }

    gsl_vector_memcpy(g, theta);

    gsl_vector_free(theta);
    gsl_vector_complex_free(y);
}

