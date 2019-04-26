#include "gci_sedreams.h"

using std::vector;

std::deque<int> findPeaks(const valarray& signal, double sign) {
    
    const int N(signal.size());

    int n;

    valarray df1(signal.shift(2) - signal.shift(1));
    valarray df2(sign * (df1.shift(2) - df1.shift(1)));
    
    df1[0] = df1[1];
    df2[0] = df2[1] = df2[2];

    // Find sign changes in df1 and negativity in df2
    std::deque<int> idx;
    for (n = 0; n < N; ++n) {
        bool df1_sgn = (df1[n] * (n+1 < N ? df1[n+1] : 0.)) <= 0;
        bool df2_neg = (n+1 < N ? df2[n+1] : 0.) < 0;

        if (df1_sgn && df2_neg) {
            idx.push_back(n);
        }
    }

    return idx;
}
