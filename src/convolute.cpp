#include "linalg.h"


double conv_int(double x, void *p) {
    auto params = static_cast<convolute_params *>(p);
    double f, g;
    
    f = GSL_FN_EVAL(params->f, params->t - x);
    g = GSL_FN_EVAL(params->g, x);
    
    return f * g;
}

double convolute(double t, void *p) {
    auto params = static_cast<convolute_params *>(p);
    params->t = t;

    gsl_function A;
    A.function = &conv_int;
    A.params = params;

    return integrate(&A, 0, 1);
}

