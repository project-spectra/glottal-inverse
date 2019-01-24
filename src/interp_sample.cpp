#include "interp_sample.h"


struct interp_sample_params {
    gsl_interp *interp;
    double *xa;
    double *ya;
    gsl_interp_accel *accel;
    bool periodic;
    double period;
};

double interp_sample_eval(double t, void *p) {
    auto *params = static_cast<interp_sample_params *>(p);

    if (params->periodic) {
        // Shift the argument to keep it in the period.
        t -= params->period * floor(t / params->period);
    }

    return gsl_interp_eval(
        params->interp,
        params->xa,
        params->ya,
        t,
        params->accel
    );
}

void *interp_sample(gsl_vector *y, size_t M, bool periodic) {
    interp_sample_params *params;

    gsl_interp *interp;
    gsl_interp_accel *accel;

    double *xa = new double[M];
    double *ya = y->data;

    for (size_t i = 0; i < M; ++i) {
        xa[i] = (double) i / (double) SAMPLE_RATE;
    }

    interp = gsl_interp_alloc(gsl_interp_linear, M);
    gsl_interp_init(interp, xa, ya, M);
    accel = gsl_interp_accel_alloc();
    
    params = new interp_sample_params;
    params->interp = interp;
    params->xa = xa;
    params->ya = ya;
    params->accel = accel;
    params->periodic = periodic;
    params->period = xa[M - 1];
    
    return params;
}
