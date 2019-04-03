#include "amgif.h"
#include "filter.h"
#include "linalg.h"


static constexpr size_t length = basis_length();

gsl_matrix *computeL() {
    gsl_matrix *L;

    size_t i, j, k; 
    double data;
    
    double H[length];

    // basis[0]
    std::fill_n(H, length, 0.);
    H[0] = 1.;
    coords(H, H);

    L = gsl_matrix_alloc(length, length);

    for (i = 0; i < length; ++i) {
        for (j = 0; j < length; ++j) {
            k = abs((int) i - (int) j);
            if (k < length) {
                data = H[k];

                gsl_matrix_set(L, i, j, data);
            }
        } 
    }

    return L;
}
