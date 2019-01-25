#include "linalg.h"
#include "amgif.h"

#include <iostream>
#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;


/* = <f conv g, h> */
struct projconv_params {
    gsl_function *f, *g, *h;
};

/* (t, s) */
double projconv_eval(double *x, size_t dim, void *p) {
    (void) dim;

    auto params = static_cast<projconv_params *>(p);

    double f = GSL_FN_EVAL(params->f, x[0] - x[1]);
    double g = GSL_FN_EVAL(params->g, x[1]);
    double h = GSL_FN_EVAL(params->h, x[0]);

    return f * g * h;
}


vector<mat_operator> computeC() {
    auto C = vector<mat_operator>();

    size_t length, mu, p, f;
    gsl_function phi_mu, phi_p, phi_f;
    gsl_monte_function integrand;
    projconv_params intParams;
    constexpr double intDomain[][2] = {
        { 0, 0 }, { 1, 1 }
    };
    
    gsl_matrix *C_mu; 
    double data, err;

    length = basis_length();

    phi_mu.function = phi_p.function = phi_f.function = basis_eval;
    phi_mu.params = &mu;
    phi_p.params = &p;
    phi_f.params = &f;

    integrand.f = projconv_eval;
    integrand.dim = 2;
    integrand.params = &intParams;
    intParams.f = &phi_p;
    intParams.g = &phi_f;
    intParams.h = &phi_mu;

    gsl_rng *rng = gsl_rng_alloc(gsl_rng_default);
    auto mcState = gsl_monte_miser_alloc(2);

    C.reserve(length);
    for (mu = 0; mu < length; ++mu) {
        C_mu = gsl_matrix_alloc(length, length);

        std::cout << "  * Computing C_mu for mu = " << mu << "..." << std::endl;
    
        auto t1 = high_resolution_clock::now(); 

        for (p = 0; p < length; ++p) {
            for (f = 0; f < length; ++f) {
                std::cout << "      p = " << p << "; f = " << f << "  \r" << std::flush;

                gsl_monte_miser_integrate(
                    &integrand,
                    intDomain[0],
                    intDomain[1],
                    integrand.dim,
                    INTEGRATE_NCALLS,
                    rng,
                    mcState,
                    &data,
                    &err
                );
                
                gsl_matrix_set(C_mu, p, f, data);
            }
        }
        
        auto t2 = high_resolution_clock::now();

        auto dur = duration_cast<duration<double>>(t2 - t1);

        std::cout << "\033[1F]\r"
                  << "  * Computed C_mu for mu = " << mu
                  << " in " << dur.count() << " seconds" << std::endl;


        C.push_back(mat_operator(C_mu, gsl_matrix_free));
    }

    return C;
}
