#include "gci_pd.h"


void selectPeaks(arma::dvec& vec, std::vector<int>& peaks, double threshold) {

    int N(vec.n_elem);

    for (int n = 0; n < N; ++n) {
        if (vec(n) > threshold) {
            peaks.push_back(n);
    
            // Remove the peak for later GOI detection by averaging the neighbouring values
            vec(n) = ((n > 0 ? vec(n - 1) : vec(n + 1))
                        + (n < N - 1 ? vec(n + 1) : vec(n - 1)))/ 2.;
        }
    }

}
