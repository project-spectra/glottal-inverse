#include <cstdio>

#include "window.h"
#include "audio.h"
#include "filter.h"
#include "gci_yaga.h"


void computeGD(const valarray& u, valarray& y)
{
    static constexpr double fwlen = 0.00045;
    static constexpr double gwlen = 0.0015;
    
    const int gw(2 * floor(gwlen * SAMPLE_RATE / 2.) + 1);

    valarray ghw(hamming(gw));
    
    valarray ghwn(ghw);
    for (int i = 0; i < gw; ++i) {
        ghwn[i] *= (gw - 1 - 2*i) / 2.;
    }

    valarray u2 = u * u;

    valarray yn, yd;
    filter_fir(ghwn, u2, yn);
    filter_fir(ghw, u2, yd);

    double eps = std::numeric_limits<double>::epsilon();
    yd[abs(yd) < eps] = 10*eps; // prevent infinities
   
    y.resize(yn.size() - gw);
    y = yn[std::slice(gw, yn.size() - gw, 1)];
    y /= valarray(yd[std::slice(gw, yd.size() - gw, 1)]);

    int toff = (gw - 1) / 2;

    /*int fw = 2 * floor(fwlen * SAMPLE_RATE / 2.) + 1;
    if (fw > 1) {
        valarray daw(hamming(fw));
        filter_fir(daw, y, y);
        y /= daw.sum();
        toff -= (fw - 1) / 2;
    }*/
   
    y.shift(toff);
}
