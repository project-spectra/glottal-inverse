#include "amgif.h"
#include "filter.h"
#include "linalg.h"


mat_operator computeL() {
    gsl_spmatrix *L;
    gsl_vector *H;
    size_t i, j, k;

    constexpr size_t length = 2 << J;

    L = gsl_spmatrix_alloc(length, length);
    H = coords(basis(0));

    double data;
    
    for (i = 0; i < length; ++i) {
        for (j = 0; j < length; ++j) {
            k = abs((int) i - (int) j);
            if (k < length) {
                data = gsl_vector_get(H, abs((int) i - (int) j));
                gsl_spmatrix_set(L, i, j, data);
            }
        } 
    }

    gsl_vector_free(H);

    return mat_operator(L, gsl_spmatrix_free);
}
