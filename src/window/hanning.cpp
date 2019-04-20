#include <cmath>
#include <gsl/gsl_math.h>
#include "window.h"

gsl_vector *hanning(const size_t L) {
    gsl_vector *w;
    size_t n, N;
    double data;
    
    w = gsl_vector_alloc(L);
    N = L - 1;

    for (n = 0; n <= N; ++n) {
        data = .5 * (1. - cos(2. * M_PI * n / static_cast<double>(N)));
        gsl_vector_set(w, n, data);
    }

    return w;
}
