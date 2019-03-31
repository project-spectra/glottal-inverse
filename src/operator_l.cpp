#include "amgif.h"
#include "filter.h"
#include "linalg.h"


mat_operator computeL() {
    gsl_matrix *L;
    size_t i, j, k;

    constexpr size_t length = basis_length();
    
    double Ht[length];

    // basis[0]
    Ht[0] = 1.;
    for (size_t i = 1; i < length; ++i) {
        Ht[i] = 0.;
    }

    double H[length];
    
    coords(Ht, H);

    L = gsl_matrix_alloc(length, length);
   
    double data;
    
    for (i = 0; i < length; ++i) {
        for (j = 0; j < length; ++j) {
            k = abs((int) i - (int) j);
            if (k < length) {
                data = H[k];
                gsl_matrix_set(L, i, j, data);
            }
        } 
    }

    return mat_operator(L, gsl_matrix_free);
}
