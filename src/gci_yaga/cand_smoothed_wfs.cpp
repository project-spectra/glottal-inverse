#include "window.h"
#include "audio.h"
#include "gci_yaga_subroutines.h"


double cand_smoothed_wfs(const valarray& u, const size_t r, const size_t p) {
    // 1ms window length
    static const size_t winLen(1. / 1000. * SAMPLE_RATE);
    static const size_t winOff(winLen / 2);

    valarray w(hanning(winLen));
   
    double sum(0.);
    for (size_t t = 0; t < winLen; ++t) {
        sum = fma(cost_ncorr(u, r - winOff + t, p - winOff + t), w[t], sum);
    }

    return sum;
}


