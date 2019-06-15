#include "gci_pd.h"


void teager(arma::dvec& vec) {
    vec.subvec(1, vec.n_elem - 2) = arma::square(vec.subvec(1, vec.n_elem - 2))
                                    - vec.subvec(0, vec.n_elem - 3) % vec.subvec(2, vec.n_elem - 1);

    vec(0) = vec(1);
    vec(vec.n_elem - 1) = vec(vec.n_elem - 2);
}
