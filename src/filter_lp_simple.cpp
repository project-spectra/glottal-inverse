#include "audio.h"
#include "filter.h"


void filter_lpf(valarray& x, const double fc) {
    const size_t N(x.size());

    const double RC = 1. / (2. * M_PI * fc);
    const double dt = 1./ static_cast<double>(SAMPLE_RATE);
    const double alpha = RC / (RC + dt);

    valarray y(N);

    y[0] = x[0];

    for (size_t i = 1; i < N; ++i) {
        y[i] = alpha * x[i] + (1 - alpha) * y[i - 1];
    }

    // replace X with the HPF'd X
    x = y;
}
