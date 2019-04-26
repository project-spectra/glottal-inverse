#include "audio.h"
#include "gci_yaga_subroutines.h"


double cost_ssqe(const valarray& gamma, const size_t r)
{
    // 0.85ms window for Sum-SQuare-Error calculation
    static const int ssqeLen(0.85 / 1000. * SAMPLE_RATE);
    static const int ssqeOff(ssqeLen / 2);

    // ideal negative-going zero crossing with unit slope
    const double step(1. / SAMPLE_RATE);

    auto y_r(gamma[std::slice(r - ssqeOff, ssqeLen, 1)]);
    
    size_t n;
    double val;

    double ssq(0.);

    for (n = 0; n < ssqeLen; ++n) {
        val = y_r[n] - step * ((int) n - ssqeOff);
        
        ssq += val * val;
    }
    
    return ssq;
}
