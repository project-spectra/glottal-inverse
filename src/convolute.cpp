#include "constants.h"
#include "linalg.h"


struct convolute_params {
    const gsl_function *f;
    const gsl_function *g;
    double t;
};


double conv_int(double x, void *p) {
    convolute_params *params;
    double f, g;
    
    params = static_cast<convolute_params *>(p);
    
    f = GSL_FN_EVAL(params->f, params->t - x);
    g = GSL_FN_EVAL(params->g, x);
    
    return f * g;
}


double convolute(const gsl_function *f, const gsl_function *g, double t) {
    convolute_params params = {
        .f = f,
        .g = g,
        .t = t,
    };

    gsl_function A;
    A.function = &conv_int;
    A.params = &params;

    return integrate(&A, 0, 1);
}
