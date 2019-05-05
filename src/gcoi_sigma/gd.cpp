#include <cstdio>

#include "window.h"
#include "audio.h"
#include "filter.h"
#include "gcoi_sigma_subs.h"


int gcoi_sigma_gd(const valarray& u, valarray& y)
{
    const int gw(2 * floor(SIGMA_gwlen * SAMPLE_RATE / 2.) + 1);

    valarray ghw(hamming(gw));
    
    valarray ghwn(ghw);
    for (int i = 0; i < gw; ++i) {
        ghwn[i] *= (gw - 1 - 2 * i) / 2.;
    }

    valarray u2 = u * u;

    valarray yn, yd;
    filter_fir(ghwn, u2, yn);
    filter_fir(ghw, u2, yd);

    double eps = std::numeric_limits<double>::epsilon();
    yd[abs(yd) < eps] = 10*eps; // prevent infinities

    y.resize(yn.size() - gw);
    y = yn[std::slice(gw, yn.size() - gw, 1)];
    y /= yd[std::slice(gw, yd.size() - gw, 1)];

    return (gw - 1) / 2;
}
