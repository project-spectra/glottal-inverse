#include "linalg.h"
#include "amgif.h"


vector<mat_operator> computeC() {
    auto C = vector<mat_operator>();

    size_t length, mu, p, f;
    gsl_function phi_mu, phi_p, phi_f;
    gsl_function conv;
    convolute_params conv_params;
    
    gsl_matrix *C_mu; 
    double data;

    length = basis_length();

    phi_mu.function = phi_p.function = phi_f.function = basis_eval;
    phi_mu.params = &mu;
    phi_p.params = &p;
    phi_f.params = &f;

    conv.function = convolute;
    conv.params = &conv_params;
    conv_params.f = &phi_p;
    conv_params.g = &phi_f;

    for (mu = 0; mu < length; ++mu) {  
        C_mu = gsl_matrix_alloc(length, length);
        C[mu] = mat_operator(C_mu, gsl_matrix_free);

        for (p = 0; p < length; ++p) {
            for (f = 0; f < length; ++f) {
                data = projection(&conv, &phi_mu); 
                gsl_matrix_set(C_mu, p, f, data);
            }
        }
    }

    return C;
}
