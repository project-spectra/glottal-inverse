#include <cmath>
#include <gsl/gsl_math.h>

#include "gci_yaga.h"


void computeSWT(gsl_vector *u, valarray& p)
{
    const size_t N(u->size);
    const size_t J(log2(N));

    const size_t j1(GSL_MIN(3, J));

    p.resize(N, 0.);

    valarray dj(N);
    valarray aj(N);    
    // a_0 = u
    valarray aj_1(u->data, N);

    std::vector<valarray> g; // starts at j=1
    std::vector<valarray> h; // starts at j=1

    swt_app_det_init(u, J, j1, g, h);

    size_t j, n, k;
        
    for (j = 0; j < j1; ++j)
    {
        for (n = 0; n < N; ++n) {
            dj[n] = 0.;
            aj[n] = 0.;

            for (k = 0; k < N; ++k) {
                dj[n] += g[j][k] * aj_1[n-k];
                aj[n] += h[j][k] * aj_1[n-k];
            }

            aj_1 = aj;
        }

        p *= dj;
    }

    // j1-th root
    p = pow(p, 1. / (double) j1);

    // half-wave rectifying
    p[p < 0.] = 0.;
}
