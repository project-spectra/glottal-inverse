#include "linalg.h"
#include "amgif.h"
#include "persist_c.h"

#include <iostream>
#include <chrono>
#include <cstring>

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;


vector<mat_operator> computeC() {
    size_t length, mu;
    gsl_spmatrix *C_mu;

    length = 2 << J;
    
    auto def = mat_operator(nullptr, gsl_spmatrix_free);
    auto C = vector<mat_operator>(length, def);

#pragma omp parallel for
    for (mu = 0; mu < length; ++mu) { 
        auto t1 = high_resolution_clock::now(); 

        C_mu = smartGetC(mu);

        auto t2 = high_resolution_clock::now();
        auto dur = duration_cast<duration<double>>(t2 - t1);

#pragma omp critical
        std::cout << "  + mu = " << mu << " , " << dur.count() << " seconds" << std::endl;

        C[mu] = mat_operator(C_mu, gsl_spmatrix_free);
    }

    return C;
}

void computeSingleC(gsl_spmatrix *C, size_t mu) {
    constexpr size_t length = 2 << J;
    gsl_vector *phi_p, *phi_f, *inter;
    size_t p, f;
    double data;

    for (p = 0; p < length; ++p) {
        phi_p = basis(p);

        for (f = 0; f < length; ++f) {
            phi_f = basis(f);

            inter = convolute(phi_p, phi_f);
            data = gsl_vector_get(inter, mu);

            gsl_spmatrix_set(C, p, f, data);

            gsl_vector_free(inter);
            gsl_vector_free(phi_f);
        }

        gsl_vector_free(phi_p);
    }
}
