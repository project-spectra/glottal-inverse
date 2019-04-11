#include "amgif.h"
#include "filter.h"
#include "linalg.h"


gsl_spmatrix *computeL() {
    size_t i, j, k; 
    double data;
    
    double H[length];

    // basis[0]
    std::fill_n(H, length, 0.);
    H[0] = 1.;
    coords(H, H);

    auto Ltr = gsl_spmatrix_alloc(length, length);

    for (i = 0; i < length; ++i) {
        for (j = 0; j < length; ++j) {
            k = abs((int) i - (int) j);
            if (k < length) {
                data = H[k];

                gsl_spmatrix_set(Ltr, i, j, data);
            }
        } 
    }

    auto L = gsl_spmatrix_ccs(Ltr);

    gsl_spmatrix_free(Ltr);
    
    return L;
}

