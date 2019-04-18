#include "derivative.h"


struct inter_args {
    gsl_interp *interp;
    double *xa;
    double *ya;
    gsl_interp_accel *acc;
};

static double inter_eval(double t, void *params);


void deriv_discrete(gsl_vector *df, gsl_vector *f, double step)
{
    const size_t len(f->size);

    double xa[len];

    for (size_t k = 0; k < len; ++k) {
        xa[k] = step * k;
    }
   
    inter_args args;
    args.interp = gsl_interp_alloc(gsl_interp_linear, WINDOW_LENGTH);
    args.acc = gsl_interp_accel_alloc();

    gsl_interp_init(args.interp, xa, f->data, len);

    gsl_function f_interp;
    f_interp.function = &inter_eval;
    f_interp.params = &args;

    double data, abserr;
    for (size_t k = 0; k < len; ++k) {
        gsl_deriv_central(&f_interp, xa[k], step, &data, &abserr);

        gsl_vector_set(df, k, data);
    }

    gsl_interp_free(args.interp);
    gsl_interp_accel_free(args.acc);
}

static double inter_eval(double t, void *params) {
    auto args = static_cast<inter_args *>(params);

    double data;

    int err = gsl_interp_eval_e(
            args->interp,
            args->xa,
            args->ya,
            t,
            args->acc,
            &data
    );

    if (err) {
        fprintf(stderr, "interp error: %s\n", gsl_strerror(err));
        throw 0;
    }

    return data;
}
