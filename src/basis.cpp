#include "linalg.h"
#include <iostream>
/* Haar wavelet family */

double wavelet_mother(double t) {
    if (0 <= t && t < 0.5) {
        return 1.0;
    }
    if (0.5 <= t && t < 1) {
        return -1.0;
    }
    return 0.0;
}

double wavelet(size_t j, size_t l, double t) {
    double a, b, y;

    a = gsl_pow_uint(2, j / 2);
    b = a * a;

    y = a * wavelet_mother(b * t - l);

    return y;
}

double scaling(double t) {
    if (0 <= t && t < 1) {
        return 1;
    }
    return 0;
}

size_t basis_index(size_t j, size_t l) {
    // It's shifted by 1 to the right.
    return gsl_pow_uint(2, j) + l;
}

double basis_eval(double t, void *params) {
    size_t index = *(size_t *) params;

    // Scaling function
    if (index == 0) {
        return scaling(t);
    }

    // Get the (j, l) indices
    size_t j = floor(log2(index));
    size_t l = index - gsl_pow_uint(2, j);

    return wavelet(j, l, t);
}

