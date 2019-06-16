#include "lpc.h"


double lpcCoeffs(const valarray& x, size_t order, valarray& lpc) {
    const int n = x.size();
    const int m = static_cast<int>(order + 1);

    valarray aut(m + 1);
    lpc.resize(m);

    double err, eps;
    int i, j;

    j = m + 1;
    while (j--) {
        double d = 0.;
        for (i = j; i < n; ++i) {
            d += x[i] * x[i - j];
        }
        aut[j] = d;
    }

    // noise floor to about -100dB:
    err = aut[0] * (1. + 1e-10);
    eps = 1e-9 * aut[0] + 1e-10;

    for (i = 0; i < m; ++i) { 
        double r = -aut[i + 1];

        if (err < eps) {
            for (j = i; j < m; ++j) {
                lpc[j] = 0.;
            }
            goto done;
        }
        
        for (j = 0; j < i; ++j) {
            r -= lpc[j] * aut[i - j];
        }
        r /= err;

        lpc[i] = r;
        for (j = 0; j < i / 2; ++j) {
            double tmp = lpc[j];

            lpc[j] += r * lpc[i - 1 - j];
            lpc[i - 1 - j] += r * tmp;
        }

        if (i & 1) {
            lpc[j] += lpc[j] * r;
        }

        err *= 1. - r * r;
    }

done:

    double g = .99;
    double damp = g;
    for (j = 0; j < m; ++j) {
        lpc[j] *= damp;
        damp *= g;
    }

    return err;
}
