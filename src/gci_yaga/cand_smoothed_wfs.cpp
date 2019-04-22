#include "window.h"
#include "constants.h"
#include "gci_yaga_subroutines.h"


// 1ms window length
static constexpr size_t winLen = 1. / 1000. * SAMPLE_RATE;

static constexpr size_t winOff = winLen / 2;


double cand_smoothed_wfs(const gsl_vector *u, const size_t r, const size_t p) {
    auto w = hanning(winLen);
   
    double sum(0.);
    for (size_t t = 0; t < winLen; ++t) {
        sum += cost_ncorr(u, r - winOff + t, p - winOff + t) * gsl_vector_get(w, t);
    }

    gsl_vector_free(w);

    return sum;
}


