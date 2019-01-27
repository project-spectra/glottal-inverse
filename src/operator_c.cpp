#include "linalg.h"
#include "amgif.h"

#include <iostream>
#include <chrono>
#include <cstring>

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;


double projconv(gsl_vector *f, gsl_vector *g, gsl_vector *h);


vector<mat_operator> computeC() {
    auto C = vector<mat_operator>();

    size_t length, mu, p, f;
    gsl_vector *phi_mu, *phi_p, *phi_f, *conv;
 
    gsl_matrix *C_mu;
    double data;

    length = basisLength();

    C.reserve(length);
    for (mu = 0; mu < length; ++mu) {
        phi_mu = basis(mu);

        C_mu = gsl_matrix_alloc(length, length);

        std::cout << "  * Computing C_mu for mu = " << mu << "..." << std::endl;
    
        auto t1 = high_resolution_clock::now(); 

        for (p = 0; p < length; ++p) {
            phi_p = basis(p);

            for (f = 0; f < length; ++f) {
                phi_f = basis(f);

                std::cout << "      p = " << p << "; f = " << f << "  \r" << std::flush;

                conv = convolute(phi_p, phi_f);
                data = projection(conv, phi_mu);

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



double projconv(gsl_vector *f, gsl_vector *g, gsl_vector *h) {
    const size_t N = f->size;

    gsl_vector *conv, *u;
    size_t i, j, s;
    double data[N];
    auto dataView = gsl_vector_view_array(data, N);

    double res;

    for (j = 0; j < N; ++j) {
        memcpy(data, f->data, j + 1);
        for (s = j + 1; s < N; ++s) {
            data[s] = 0.;
        }
        
        gsl_vector_mul(&dataView.vector, g);
    }

    gsl_vector_mul(&dataView.vector, g);

    return integrate(&dataView.vector);
}


