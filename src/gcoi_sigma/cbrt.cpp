#include "gcoi_sigma_subs.h"

#include <armadillo>


valarray gcoi_sigma_cbrt(const valarray& x) {
    const int N(x.size());

    valarray y(N);

    for (int i = 0; i < N; ++i) {
        y[i] = std::pow(x[i], 1./3.);
    }

    return y;
}
