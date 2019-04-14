#include "amgif.h"
#include "filter.h"
#include "linalg.h"


gsl_matrix *computeL() {
    size_t i, j, k; 
    
    auto& H = getBasis(0);
    
    auto L = gsl_matrix_alloc(basisLength, basisLength);

    for (i = 0; i < basisLength; ++i) {
        for (j = 0; j < basisLength; ++j) {
            k = abs((int) i - (int) j);
            if (k < basisLength) {
                gsl_matrix_set(L, i, j, H[k]);
            }
        } 
    }
    
    return L;
}

