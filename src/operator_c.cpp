#include "linalg.h"
#include "amgif.h"
#include "restore_c.h"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstring>

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;


#ifndef PRECOMP
ListCmu
#else
void
#endif
computeC() {

    ComputeStatus toLoad, toStore;

    auto C = vector<mat_operator>();
    C.resize(length);

    findComputeStatus(toLoad, toStore);

#ifdef PRECOMP
    std::cout << "  + Skipping already computed elements..." << std::endl;
#else
    std::cout << "  + Loading already computed elements..." << std::endl;
    
    #pragma omp parallel for schedule(dynamic)
    for (auto it = toLoad.cbegin(); it < toLoad.cend(); ++it) {
        size_t mu = it->first;
        auto C_mu = smartGetC(mu, it->second, true); // load

        C[mu] = mat_operator(C_mu, gsl_spmatrix_free);
    }

#endif

    std::cout << std::endl << std::setprecision(3) << std::fixed;

    #pragma omp parallel for schedule(dynamic)
    for (auto it = toStore.cbegin(); it < toStore.cend(); ++it) {
        size_t mu = it->first;
        
        auto t1 = high_resolution_clock::now();

        auto C_mu = smartGetC(mu, it->second, false); // store
        
        auto t2 = high_resolution_clock::now();
        auto dur = duration_cast<duration<double>>(t2 - t1);

        #pragma omp critical
        std::cout << "  + mu = " << mu << " in " << dur.count() << " seconds" << std::endl;

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
    size_t p, f;
    double data;

    #pragma omp parallel private(p, f)
    for (p = 0; p < length; ++p) {
        #pragma omp for
        for (f = p; f < length; ++f) {
            data = convoluteBasis(p, f, mu);
            
            #pragma omp critical
            {
                gsl_spmatrix_set(C, p, f, data);
                gsl_spmatrix_set(C, f, p, data);
            } 
        }
    }
}
