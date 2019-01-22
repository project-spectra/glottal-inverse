#include "linalg.h"


static gsl_integration_workspace *workspace = nullptr; 


double integrate(const gsl_function *f, double a, double b) {
    if (workspace == nullptr) {
        workspace = gsl_integration_workspace_alloc(INTEGRATE_WORK);
    }   

    double result, abserr;
    
    gsl_integration_qag(
            f, a, b,
            INTEGRATE_EPSABS, INTEGRATE_EPSREL,
            INTEGRATE_WORK, GSL_INTEG_GAUSS21,
            workspace, &result, &abserr
    );

    return result;
}

void integrate_dispose() {
    if (workspace != nullptr) {
        gsl_integration_workspace_free(workspace);
    }
}
