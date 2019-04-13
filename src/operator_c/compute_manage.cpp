#include "operators.h"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <string>

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::duration_cast;


// Digit count basis length
const size_t basisMaxDigits =
    // log10(2^(J+1)) = log2(2^(J+1))/log2(10) = (J+1) / log2(10)
    ceil((J + 1) / log2(10.)) + 1;


static inline std::string percent(size_t a, size_t b) {
    size_t per = round((100. * a) / static_cast<double>(b));
    
    return std::to_string(per) + '%';
}


void computeC() {

    vector<bool> toStore;

    findValidCs(toStore);
  
    std::cout << "    * Total elements to compute (or not): " << basisLength << std::endl;
    std::cout << std::setprecision(2) << std::fixed;

    size_t totalDone(0);

    #pragma omp parallel for schedule(auto) shared(toStore)
    for (size_t mu = 0; mu < basisLength; ++mu) {
    
        #pragma omp critical
        totalDone++;

        if (!toStore[mu]) {
            #pragma omp critical
            std::cout << "[" << std::setw(4) << percent(totalDone, basisLength) << "]"
                      << "   + mu = " << std::left << std::setw(basisMaxDigits) << mu
                      << "   (precomputed)" << std::endl;
            continue;
        }
      
        auto t1 = high_resolution_clock::now();

        auto C_mu = computeSingleC(mu);
        storeMat(C_mu, mu);
        
        auto t2 = high_resolution_clock::now();
        auto dur = duration_cast<duration<double>>(t2 - t1);

        #pragma omp critical
        std::cout << "[" << std::setw(4) << percent(totalDone, basisLength) << "]"
                  << "   + mu = " << std::left << std::setw(basisMaxDigits) << mu
                  << "   in   " << dur.count() << " seconds" << std::endl;

        gsl_spmatrix_free(C_mu);
    }

}



