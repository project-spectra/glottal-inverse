#include "constants.h"
#include "gci_yaga_subroutines.h"

// 0.85ms window for Sum-SQuare-Error calculation
static constexpr size_t ssqeLen = 0.85 / 1000. * SAMPLE_RATE;
static constexpr size_t ssqeOff = ssqeLen / 2;

// ideal negative-going zero crossing with unit slope
static constexpr double step = 1. / SAMPLE_RATE;


double cost_ssqe(const valarray& gamma, const size_t r)
{
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
