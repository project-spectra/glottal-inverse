#include "linalg.h"
#include "amgif.h"

#include <iostream>
#include <chrono>
#include <cstring>

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;


vector<mat_operator> computeC() {
    auto C = vector<mat_operator>();

    size_t length, mu, p, f;
    gsl_vector *phi_p, *phi_f, *inter;
 
    gsl_matrix *C_mu;
    double data;

    length = 2 << J;

    C.reserve(length);
    for (mu = 0; mu < length; ++mu) {
        C_mu = gsl_matrix_alloc(length, length);

        std::cout << "  * Computing C_mu for mu = " << mu << "..." << std::flush;
    
        auto t1 = high_resolution_clock::now(); 

        for (p = 0; p < length; ++p) {
            phi_p = basis(p);

            for (f = 0; f < length; ++f) {
                phi_f = basis(f);

                inter = convolute(phi_p, phi_f);
                data = gsl_vector_get(inter, mu);

                gsl_matrix_set(C_mu, p, f, data);

                gsl_vector_free(inter);
                gsl_vector_free(phi_f);
            }
            gsl_vector_free(phi_p);
        }

        auto t2 = high_resolution_clock::now();

        auto dur = duration_cast<duration<double>>(t2 - t1);

        std::cout << "\r  * Computed C_mu for mu = " << mu
                  << " in " << dur.count() << " seconds   " << std::endl;


        C.push_back(mat_operator(C_mu, gsl_matrix_free));
    }

    return C;
}

