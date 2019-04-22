#include <cmath>
#include <gsl/gsl_math.h>

#include "gci_yaga.h"


void computeSWT(gsl_vector *u, valarray& p)
{
    const size_t N(u->size);
    const size_t J(log2(N) - 1);

    constexpr int j0(1);
    const int j1(GSL_MIN(3, J));

    const valarray h = { 1.586134342, -.05298011854, -.8829110762, .4435068522, 1.149604398 };
    const size_t m(h.size() / 2);

    std::vector<valarray> d(j1 + 1, valarray(u->data, N));
    
    int j;

    valarray xj(u->data, N);
    valarray dj;

    p.resize(N, 1.);

    for (j = j1; j >= j0; --j) {
        int dist = pow(2, j1 - j);

        std::valarray<size_t> s1(N);
        std::valarray<size_t> s2(N);
        for (int n = 0; n < N; ++n) {
            int sn;

            // Boundary conditions
            sn = n + dist;
            if (sn >= N)
                sn = 2*N - sn - 1;
            else if (sn < 0)
                sn = 1 - sn;
            s1[n] = sn;

            sn = n - dist;
            if (sn >= N)
                sn = 2*N - sn - 1;
            else if (sn < 0)
                sn = 1 - sn;
            s2[n] = sn;
        }
        
        dj = xj;

        for (int i = 0; i < m; ++i) {
            dj -= h[2*i] * (valarray(xj[s1]) + valarray(xj[s2]));
            xj += h[2*i+1] * (valarray(dj[s1]) + valarray(dj[s2]));
        }

        dj *= h[h.size()-1];
        xj = dj;

        p *= pow(dj, 1. / (double) j1);
    }

    // half-wave rectifying
    for (size_t n = 0; n < N; ++n)
        if (p[n] < 0. || !isfinite(p[n]))
            p[n] = 0.;

}
