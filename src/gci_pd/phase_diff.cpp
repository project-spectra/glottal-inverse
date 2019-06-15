#include "gci_pd.h"


using namespace arma;


dvec phaseDifference(const cx_vec& vec) {

    const int N(vec.n_elem);

    dvec theta = zeros<dvec>(N / 2);

    theta.tail(N / 2 - 1) = diff(arg(vec.head(N / 2)));
    theta(0) = theta(1);

    return theta;

}
