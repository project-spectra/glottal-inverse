#include "linalg.h"
#include <iostream>

double conv_int(double s, void *p) {
    auto params = static_cast<convolute_params *>(p);
    double f, g;
    
    f = GSL_FN_EVAL(params->f, params->t - s);
    g = GSL_FN_EVAL(params->g, s);
    
    return f * g;
}

double convolute(double t, void *p) {
    auto params = static_cast<convolute_params *>(p);

    auto conv_params = new convolute_params;
    conv_params->f = params->f;
    conv_params->g = params->g;
    conv_params->t = t;

    gsl_function A;
    A.function = conv_int;
    A.params = conv_params;

    return integrate(&A, 0, 1);
}

