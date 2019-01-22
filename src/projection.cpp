#include "linalg.h"


double proj_int(double x, void *p) {
    projection_params *params;
    double f, g;
    
    params = static_cast<projection_params *>(p);
    
    f = GSL_FN_EVAL(params->f, x);
    g = GSL_FN_EVAL(params->g, x);
    
    return f * g;
}


double projection(gsl_function *f, gsl_function *g) {
    projection_params params;
    params.f = f;
    params.g = g;

    gsl_function A;
    A.function = proj_int;
    A.params = &params;

    return integrate(&A, 0, 1);
}
