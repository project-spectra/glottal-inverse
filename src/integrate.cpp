#include "linalg.h"


double integrate(const gsl_function *f, double a, double b) {
    double result, abserr;
    
    auto work = gsl_integration_workspace_alloc(2000);

    gsl_integration_qag(
            f, a, b,
            INTEGRATE_EPSABS, INTEGRATE_EPSREL,
            INTEGRATE_WORK, GSL_INTEG_GAUSS21,
            work, &result, &abserr
    );

    gsl_integration_workspace_free(work);

    return result;
}

