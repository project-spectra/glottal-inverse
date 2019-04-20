#include "gci_sedreams.h"

using std::vector;

std::deque<size_t> findPeaks(gsl_vector *signal, double sign) {
    
    const size_t N(signal->size);

    size_t n;

    vector<double> df1(signal->size);
    for (n = 1; n < N; ++n)
        df1[n] = gsl_vector_get(signal, n) - gsl_vector_get(signal, n-1);

    vector<double> df2(signal->size);
    for (n = 2; n < N; ++n)
        df2[n] = sign * (df1[n] - df1[n-1]);
    
    df1[0] = df1[1];
    df2[0] = df2[1] = df2[2];

    // Find sign changes in df1 and negativity in df2
    std::deque<size_t> idx;
    for (n = 0; n < N; ++n) {
        bool df1_sgn = (df1[n] * (n+1 < N ? df1[n+1] : 0.)) <= 0;
        bool df2_neg = (n+1 < N ? df2[n+1] : 0.) < 0;

        if (df1_sgn && df2_neg) {
            idx.push_back(n);
        }
    }

    return idx;

    /* 
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
    */
}
