#include "constants.h"
#include "linalg.h"


struct projection_params {
    const gsl_function *f;
    const gsl_function *g;
};


double proj_int(double x, void *p) {
    projection_params *params;
    double f, g;
    
    params = static_cast<projection_params *>(p);
    
    f = GSL_FN_EVAL(params->f, x);
    g = GSL_FN_EVAL(params->g, x);
    
    return f * g;
}


double projection(const gsl_function *f, const gsl_function *g) {
    projection_params params = {
        .f = f,
        .g = g,
    };

    gsl_function A;
    A.function = &proj_int;
    A.params = &params;

    return integrate(&A, 0, 1);
}
