#include <cstdio>

#include "window.h"
#include "constants.h"
#include "gci_yaga.h"


// 2ms window size
static constexpr size_t L = 2. / 1000. * SAMPLE_RATE;


void computeGD(const valarray& pm, valarray& gamma)
{
    const size_t N(pm.size());

    valarray w(hanning(L));

    std::vector<valarray> x(N);

    for (size_t n = 0; n < N; ++n) {
        x[n] = w;
        x[n] *= pm[std::slice(n, (L < N + 1 - n) ? (N + 1 - n) : L, 1)];
    }
    
    gamma.resize(N);

    valarray range(L);
    for (size_t l = 0; l < L; ++l) {
        range[l] = l;
    }

    for (size_t n = 0; n < N; ++n)
    {
        valarray xn2(x[n] * x[n]);

        gamma[n] = (range * xn2).sum() / xn2.sum() - (L-1)/2;
    }
}
