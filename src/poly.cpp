#include "poly.h"

using namespace arma;

cx_vec poly(const cx_vec& z)
{
    const uword n(z.n_elem);
    cx_vec p = zeros<cx_vec>(n + 1);

    p(0) = 1.;

    for (size_t j = 0; j < n; ++j) {
        p(span(1, j + 1)) -= z(j) * p(span(0, j));
    }

    return p;
}

