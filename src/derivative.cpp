#include <gsl/gsl_interp.h>
#include "constants.h"
#include "derivative.h"


void deriv_discrete(gsl_vector *df, gsl_vector *f, double step)
{
    const size_t len(f->size);

    double xa[len];
    size_t k;

    for (k = 0; k < len; ++k) {
        xa[k] = step * k;
    }
   
    auto interp = gsl_interp_alloc(gsl_interp_linear, WINDOW_LENGTH);
    auto acc = gsl_interp_accel_alloc();

    gsl_interp_init(interp, xa, f->data, len);

    double data;

    for (k = 0; k < len; ++k) {
        data = gsl_interp_eval_deriv(interp, xa, f->data, xa[k], acc);

        gsl_vector_set(df, k, data);
    }

    gsl_interp_free(interp);
    gsl_interp_accel_free(acc);
}

