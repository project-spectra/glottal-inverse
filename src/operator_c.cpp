#include "linalg.h"
#include "amgif.h"

#include <iostream>
#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;

void convoluteBasis(size_t i, size_t j, gsl_vector *conv) {
    if (i == j) {
        gsl_vector_set_basis(conv, i);
    } else {
        gsl_vector_set_zero(conv);
    }
}

ListCmu computeC() {
    static constexpr size_t length = basis_length();

    auto C = vector<mat_operator>();

    size_t mu, p, f;
    
    gsl_matrix *C_mu;
    double data;

    auto conv = gsl_vector_alloc(length);
    
    C.reserve(length);
    for (mu = 0; mu < length; ++mu) {
        C_mu = gsl_matrix_alloc(length, length);

        std::cout << "  * Computing C_mu for mu = " << mu << "..." << std::endl;
    
        auto t1 = high_resolution_clock::now(); 

        for (p = 0; p < length; ++p) {
//            std::cout << "      p = " << p << "   \r" << std::flush;

            for (f = 0; f < length; ++f) {

                convoluteBasis(p, f, conv);
                
                data = gsl_vector_get(conv, mu);
                
                gsl_matrix_set(C_mu, p, f, data);
                gsl_matrix_set(C_mu, f, p, data);
            }
        }
        
        auto t2 = high_resolution_clock::now();

        auto dur = duration_cast<duration<double>>(t2 - t1);

        std::cout << "\033[1F]\r"
                  << "  * Computed C_mu for mu = " << mu
                  << " in " << dur.count() << " seconds" << std::endl;


        C.push_back(mat_operator(C_mu, gsl_matrix_free));
    }

    gsl_vector_free(conv);

    return C;
}
