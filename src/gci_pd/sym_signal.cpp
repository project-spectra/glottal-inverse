#include "gci_pd.h"


arma::cx_vec symSignal(const arma::dvec& x) {

    int N(x.n_elem);

    arma::cx_vec y = arma::zeros<arma::cx_vec>(2 * N);

    for (int n = 0; n < N; ++n)
        y(n) = x(n);
    for (int n = N; n < 2*N; ++n)
        y(n) = x(-n + 2*N - 1);

    return y;
}
