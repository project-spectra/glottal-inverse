#include "linalg.h"


double convolutedBasis(size_t p, size_t f, size_t mu) {

    auto& conv = getConvoluted(p, f);

    return conv[mu];

}
