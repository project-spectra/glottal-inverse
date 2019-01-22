#include <gsl/gsl_vector.h>

#define delete extern_delete
#include <gsl/gsl_filter.h>
#undef extern_delete

#include "iaif.h"
#include "audio_in.h"
#include "constants.h"


void computeIAIF(sample *inputData, size_t numSamples) {
    gsl_vector *x = gsl_vector_alloc(numSamples);
    int p_vt = (2 * SAMPLE_RATE) / 2000 + 4;
    int p_gl = (2 * SAMPLE_RATE) / 4000;
    double d = 0.99;
    int hpfilt = 1;

    for (size_t i = 0; i < numSamples; ++i) {
        gsl_vector_set(x, i, inputData[i]);
    }

    auto workspace = gsl_filter_gaussian_alloc(numSamples);
    constexpr double alpha = 0.3;

    int preflt = p_vt + 1;

    if (numSamples > p_vt) {
        // Window the signal.
        gsl_filter_gaussian(GSL_FILTER_END_PADVALUE, alpha, 0, x, x, workspace);

    }
}
