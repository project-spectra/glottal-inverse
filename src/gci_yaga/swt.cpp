#include <cmath>
#include <gsl/gsl_math.h>

#include "gci_yaga.h"


void computeSWT(gsl_vector *u, valarray& p)
{
    const size_t N(u->size);
    const size_t J(log2(N));

    const size_t j1(GSL_MIN(3, J));

    const valarray h = { M_SQRT2 / 2., M_SQRT2 / 2., 0. };
    const valarray g = { 0., M_SQRT2 / 2., M_SQRT2 / 2. };

    const size_t nh1(h.size() - 1);
    const size_t ng1(g.size() - 1);

    valarray hj;
    valarray gj;

    valarray dj(N);
    valarray aj(N);    

    // a_0 = u
    valarray aj_1(u->data, N);

    p.resize(N, 1.);

    size_t j, n, k;
     
    for (j = 1; j <= j1; ++j)
    {
        size_t nz = pow(2, j);
        hj.resize(nz * nh1 + 1, 0.);
        gj.resize(nz * ng1 + 1, 0.);

        hj[std::slice(0, nz * nh1, nz)] = h;
        gj[std::slice(0, nz * ng1, nz)] = g;

        for (n = 0; n < N; ++n) {
            dj[n] = 0.;
            aj[n] = 0.;

            for (k = 0; k < hj.size(); ++k) {
                dj[n] = fma(gj[k], aj_1[n-k], dj[n]);
                aj[n] = fma(hj[k], aj_1[n-k], aj[n]);
            }

            aj_1 = aj;
        }

        // j1-th root
        p *= pow(dj, 1. / (double) j1);
    }

    // half-wave rectifying
    p[p < 0.] = 0.;

    for (size_t n = 0; n < N; ++n)
        if (p[n] != 0.)
            printf("p[%zu] = %g\n", n, p[n]);

}
