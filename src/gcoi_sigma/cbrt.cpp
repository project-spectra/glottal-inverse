#include "gcoi_sigma_subs.h"


valarray gcoi_sigma_cbrt(const valarray& x) {
    const int N(x.size());

    valarray y(N);

    for (int i = 0; i < N; ++i) {
        y[i] = std::cbrt(x[i]);
    }

    return y;
}
