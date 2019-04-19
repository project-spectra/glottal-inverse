#include "gci_sedreams.h"


std::deque<size_t> findPeaks(gsl_vector *signal, double sign) {

    std::deque<size_t> peaks;
    
    double prevVal = -HUGE_VAL;
    
    bool ascending = true;

    for (size_t n = 1; n < signal->size; ++n) {

        double curVal = sign * gsl_vector_get(signal, n);

        // (still) ascending?
        if (prevVal < curVal) {
            ascending = true;
        } 
        // (still) descending?
        else if (prevVal > curVal) {
            // starts descending?
            if (ascending) {
                 peaks.push_back(n-1); 
                 ascending = false;
            }
        }
        // prevVal == curVal is simply ignored...
        
        prevVal = curVal;
    }

    return peaks;
}
