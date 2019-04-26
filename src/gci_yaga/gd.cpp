#include <cstdio>

#include "window.h"
#include "audio.h"
#include "gci_yaga.h"


void computeGD(const valarray& pm, valarray& gamma)
{
    // 2ms window size
    static const int L(2. / 1000. * SAMPLE_RATE);

    const int N(pm.size());

    valarray w(hamming(L));

    std::vector<valarray> x(N);

    for (int n = 0; n < N; ++n) {
        x[n] = w;
        x[n] *= pm[std::slice(n, (L < N + 1 - n) ? (N + 1 - n) : L, 1)];
    }
    
    gamma.resize(N);

    valarray range(L);
    for (int l = 0; l < L; ++l) {
        range[l] = l;
    }

    for (int n = 0; n < N; ++n)
    {
        valarray xn2(x[n] * x[n]);

        gamma[n] = (range * xn2).sum() / xn2.sum() - (L-1)/2;
    }
}
