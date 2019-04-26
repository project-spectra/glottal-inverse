#include "audio.h"
#include "filter.h"
#include "gci_yaga_subroutines.h"


void cand_select_frobs(const valarray& u, double& maxFN, valarray& FNs)
{
    // Prediction order
    static const int p(1. / 1000. * SAMPLE_RATE);

    // 3ms of normalized amplitude segment
    static const int m(3. / 1000. * SAMPLE_RATE);

    // Speech segment offset
    static const int offset(1.5 / 1000 * SAMPLE_RATE);

    valarray range(p);
    for (size_t k = 0; k < p; ++k)
        range[k] = k+1;

    valarray w(m + p, p + 1);
    w[std::slice(0, p, 1)] = range;
    w[std::slice(m+1, p+1, 1)] = range[std::slice(p-1, p, -1)];
    
    w /= (p + 1.);

    filter_fir(w, u, FNs);

    maxFN = FNs.max();

}
