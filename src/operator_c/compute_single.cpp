#include "operators.h"
#include "linalg.h"


gsl_spmatrix *computeSingleC(size_t mu) {

    auto C = gsl_spmatrix_alloc(basisLength, basisLength);

    size_t p, f;
    double data;

    for (p = 0; p < basisLength; ++p) {
        for (f = p; f < basisLength; ++f) {
            data = convolutedBasis(p, f, mu);

            gsl_spmatrix_set(C, p, f, data);
            gsl_spmatrix_set(C, f, p, data);
        }
    }

    auto Cc = gsl_spmatrix_ccs(C);

    gsl_spmatrix_free(C);

    return Cc;

}
