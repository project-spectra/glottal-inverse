#include "linalg.h"
#include "amgif.h"
#include "persist_c.h"

#include <iostream>
#include <chrono>
#include <cstring>

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;


#ifndef PRECOMP
vector<mat_operator>
#else
void
#endif
computeC() {
    constexpr size_t length = 2 << J;
    size_t mu;
    gsl_spmatrix *C_mu;

    ComputeStatus toLoad, toStore;
   
#ifndef PRECOMP
    auto def = mat_operator(nullptr);
    auto C = vector<mat_operator>(length, def);
#endif

    findComputeStatus(toLoad, toStore);

#ifndef PRECOMP
    for (auto it = toLoad.cbegin(); it < toLoad.cend(); ++it) {
        mu = it->first;
        
        C_mu = smartGetC(mu, it->second, true); // load

        std::cout << "  + mu = " << mu << " (precomputed)" << std::endl;
        
        C[mu] = mat_operator(C_mu, gsl_spmatrix_free);
    }
#else
    std::cout << "  + Skipping already computed elements" << std::endl;
#endif

#pragma omp parallel for schedule(guided) private(mu, C_mu)
    for (auto it = toStore.cbegin(); it < toStore.cend(); ++it) {
        mu = it->first;
        
        auto t1 = high_resolution_clock::now(); 

        C_mu = smartGetC(mu, it->second, false); // load

        auto t2 = high_resolution_clock::now();
        auto dur = duration_cast<duration<double>>(t2 - t1);

#pragma omp critical
        std::cout << "  + mu = " << mu << " , " << dur.count() << " seconds" << std::endl;

#ifndef PRECOMP
#pragma omp critical
        C[mu] = mat_operator(C_mu, gsl_spmatrix_free);
#else
        gsl_spmatrix_free(C_mu);
#endif
    }

#ifndef PRECOMP
    return C;
#endif
}

void computeSingleC(gsl_spmatrix *C, size_t mu) {
    constexpr size_t length = 2 << J;
    gsl_vector *phi_p, *phi_f, *inter;
    size_t p, f;
    double data;

#pragma omp for schedule(guided) collapse(1) private(phi_p, phi_f, inter, p, f, data)
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
