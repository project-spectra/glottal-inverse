#include "filter.h"
#include <cstring>

void filter_fir(gsl_vector *b, gsl_vector *x, gsl_vector *y) {
    size_t m, n;
    double data;

    for (m = 0; m < x->size; ++m) {
        data = 0.0;
        
        for (n = 0; n < b->size && n <= m; ++n) {
            data += gsl_vector_get(b, n) * gsl_vector_get(x, m - n);
        }

        gsl_vector_set(y, m, data);
    }
}

void filter_iir(gsl_vector *b, gsl_vector *a, gsl_vector *x) {
    double x_tmp[x->size];
    memcpy(x_tmp, x->data, x->size * sizeof(double));

    size_t m, n;
    double data;

    for (m = 0; m < x->size; ++m) {
        data = 0.0;
        
        for (n = 0; n < b->size && n <= m; ++n) {
            data += gsl_vector_get(b, n) * x_tmp[m - n];
        }
        
        for (n = 1; n < a->size && n <= m; ++n) {
            data -= gsl_vector_get(a, n) * gsl_vector_get(x, m - n);
        }

        gsl_vector_set(x, m, data);
    }
}

