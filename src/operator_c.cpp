#include "linalg.h"
#include "amgif.h"
#include "persist_c.h"

#include <iostream>
#include <chrono>
#include <cstring>

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;


static constexpr size_t length = basis_length();

#ifndef PRECOMP
ListCmu
#else
void
#endif
computeC() {

    ComputeStatus toLoad, toStore;
    size_t mu;
    gsl_spmatrix *C_mu;

#ifndef PRECOMP
    auto C = vector<mat_operator>();
    C.resize(length);
#endif

    findComputeStatus(toLoad, toStore);

#ifdef PRECOMP
    std::cout << "  + Skipping already computed elements" << std::endl;
#else
    for (auto it = toLoad.cbegin(); it < toLoad.cend(); ++it) {
        
        mu = it->first;
        C_mu = smartGetC(mu, it->second, true); // load

        C[mu] = mat_operator(C_mu, gsl_spmatrix_free);

        std::cout << "  + mu = " << mu << " (precomputed)" << std::endl;
    }
#endif

    for (auto it = toStore.cbegin(); it < toStore.cend(); ++it) {
        mu = it->first;
        
        auto t1 = high_resolution_clock::now();

        C_mu = smartGetC(mu, it->second, false); // store
        
        auto t2 = high_resolution_clock::now();
        auto dur = duration_cast<duration<double>>(t2 - t1);
        
        std::cout << "  + mu = " << mu << " , " << dur.count() << " seconds" << std::endl;

#ifdef PRECOMP
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
    size_t p, f;
    double data;

    for (p = 0; p < length; ++p) {
        for (f = p; f < length; ++f) {
            auto conv = convoluteBasis(p, f);
            
            data = gsl_vector_get(conv.get(), mu);
            
            gsl_spmatrix_set(C, p, f, data);
            gsl_spmatrix_set(C, f, p, data);
        } 
    }
}
