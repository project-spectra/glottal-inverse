#include "constants.h"
#include "linalg.h"

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
    // for phi_1
    if (j > J) {
        return TAU * t;
    }

    double a, b, y;

    a = gsl_pow_uint(2, j / 2);
    b = a * a;

    y = a * wavelet_mother(b * t - l);

    return y;
}

