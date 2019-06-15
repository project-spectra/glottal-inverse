#include "audio.h"
#include "gci_pd.h"


void reduceVTI(arma::dvec& vec) {

    static const double alpha = .5;

    // Low-pass filter.
    arma::dvec out = arma::zeros(vec.n_elem);
    out(0) = alpha * vec(0);
    for (int i = 1; i < vec.n_elem; ++i) {
        out(i) = out(i - 1) + alpha * (vec(i) - out(i - 1));
    }

    vec = std::move(out);
}
