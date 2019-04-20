#include <gsl/gsl_math.h>
#include "constants.h"
#include "filter.h"


void filter_hpf(gsl_vector *x, const double fc) {
    const size_t N = x->size;

    const double RC = 1. / (2. * M_PI * fc);
    const double dt = 1./ static_cast<double>(SAMPLE_RATE);
    const double alpha = RC / (RC + dt);

    auto y = gsl_vector_alloc(N);
    
    gsl_vector_set(y, 0, gsl_vector_get(x, 0));

    size_t i;
    double data;

    for (i = 1; i < N; ++i) {
        data = alpha * (gsl_vector_get(y, i - 1)
                        + gsl_vector_get(x, i)
                        - gsl_vector_get(x, i - 1));

        gsl_vector_set(y, i, data);
    }

    // replace X with the HPF'd X
    gsl_vector_memcpy(x, y);
    gsl_vector_free(y);

}
