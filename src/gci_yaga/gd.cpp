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
        std::slice safeSlice(n, (n + L - 1 < N) ? L : (N - n + 1), 1);

        x[n].resize(L);
        x[n] = w;
        x[n] *= pm[safeSlice];
    }
    
    gamma.resize(N);
    for (size_t n = 0; n < N; ++n)
    {
        valarray xn2 = x[n] * x[n];

        double lxn2sum(0.), xn2sum(0.);
        for (size_t k = 0; k < L; ++k) {
            lxn2sum += k * xn2[k];
            xn2sum += xn2[k];
        }

        gamma[n] = lxn2sum / xn2sum - (L-1)/2;
    }
}
