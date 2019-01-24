#include "filter.h"


gsl_vector *filter_fir(gsl_vector *b, size_t N, gsl_vector *x, size_t M) {
    return filter_iir(b, N, nullptr, 1, x, M);
}


gsl_vector *filter_iir(gsl_vector *b, size_t Nb, gsl_vector *a, size_t Na, gsl_vector *x, size_t M) {
    size_t m, n;
    double data;

    gsl_vector *y = gsl_vector_alloc(M);

    for (m = 0; m < M; ++m) {
        data = 0.0;
        
        for (n = 0; n < Nb && n <= m; ++n) {
            data += gsl_vector_get(b, n) * gsl_vector_get(x, m - n);
        }
        
        for (n = 1; n < Na && n <= m; ++n) {
            data -= gsl_vector_get(a, n) * gsl_vector_get(y, m - n);
        }

        gsl_vector_set(y, m, data);
    }

    return y;
}

