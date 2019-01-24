#include "linalg.h"

#include <iostream>

double integrate(const gsl_function *f, double a, double b) {
    double result;
    size_t neval;
    
    static auto work = gsl_integration_romberg_alloc(INTEGRATE_WORK);

    gsl_integration_romberg(
            f, a, b,
            INTEGRATE_EPSABS, INTEGRATE_EPSREL,
            &result, &neval, work
    );

    //gsl_integration_romberg_free(work);

    return result;
}

