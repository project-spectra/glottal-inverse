#include "amgif.h"
#include "filter.h"
#include "linalg.h"


gsl_spmatrix *computeL() {
    size_t i, j, k; 
    double data;
    
    gsl_vector *H = getBasis(0);
    
    auto Ltr = gsl_spmatrix_alloc(basisLength, basisLength);

    for (i = 0; i < basisLength; ++i) {
        for (j = 0; j < basisLength; ++j) {
            k = abs((int) i - (int) j);
            if (k < basisLength) {
                data = gsl_vector_get(H, k);

                gsl_spmatrix_set(Ltr, i, j, data);
            }
        } 
    }

    auto L = gsl_spmatrix_ccs(Ltr);

    gsl_spmatrix_free(Ltr);
    gsl_vector_free(H);
    
    return L;
}

