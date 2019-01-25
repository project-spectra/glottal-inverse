#include "filter.h"


gsl_vector *filter_fir(gsl_vector *b, gsl_vector *x) {
    size_t m, n;
    double data;

    gsl_vector *y = gsl_vector_alloc(x->size);

    for (m = 0; m < x->size; ++m) {
        data = 0.0;
        
        for (n = 0; n < b->size && n <= m; ++n) {
            data += gsl_vector_get(b, n) * gsl_vector_get(x, m - n);
        }

        gsl_vector_set(y, m, data);
    }
    
    return y;
}


gsl_vector *filter_iir(gsl_vector *b, gsl_vector *a, gsl_vector *x) {
    size_t m, n;
    double data;

    gsl_vector *y = gsl_vector_alloc(x->size);

    for (m = 0; m < x->size; ++m) {
        data = 0.0;
        
        for (n = 0; n < b->size && n <= m; ++n) {
            data += gsl_vector_get(b, n) * gsl_vector_get(x, m - n);
        }
        
        for (n = 1; n < a->size && n <= m; ++n) {
            data -= gsl_vector_get(a, n) * gsl_vector_get(y, m - n);
        }

        gsl_vector_set(y, m, data);
    }

    return y;
}

