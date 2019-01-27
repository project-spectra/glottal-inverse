#include "linalg.h"
#include <iostream>

/* Haar wavelet family */

static constexpr size_t NSAMPLES = WINDOW_LENGTH * SAMPLE_RATE;

// wavelets

struct arrvec {
    double data[NSAMPLES];
    gsl_vector_view view;
};

static arrvec wavelets[basisLength()];

static double motherWavelet(double t) {
    if (0 <= t && t < .5) {
        return 1.;
    }
    if (.5 <= t && t < 1) {
        return -1.;
    }
    return 0.;
}

void constructScaling(arrvec& y) {
    size_t k;
    for (k = 0; k < NSAMPLES; ++k) {
        y.data[k] = 1.;
    }
    y.view = gsl_vector_view_array(y.data, NSAMPLES);
}

void constructWavelet(size_t j, size_t l, arrvec& y) {
    size_t k;
    double a, b;

    a = 2 << (j / 2);
    b = 2 << j;
    
    for (k = 0; k < NSAMPLES; ++k) {
        y.data[k] = a * motherWavelet(b * k / (double) NSAMPLES - l);
    }
    y.view = gsl_vector_view_array(y.data, NSAMPLES);
}

void constructBasis() {
    constexpr size_t length = basisLength();

    size_t index;

    constructScaling(wavelets[0]);

    for (index = 1; index < length; ++index) {
        // Get the (j, l) indices
        size_t j = floor(log2(index));
        size_t l = index - gsl_pow_uint(2, j);

        constructWavelet(j, l, wavelets[index]);
    }
}

gsl_vector *basis(size_t index) {
    auto res = &wavelets[index].view.vector;

    return res;
}

