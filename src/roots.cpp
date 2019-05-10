#include <armadillo>

#include "roots.h"

using namespace arma;

cvalarray roots(const valarray& p)
{
    const int n = p.size() - 1;

    dmat A = zeros<dmat>(n, n);
    A.submat(span(1, n - 1), span(0, n - 2)).diag() = ones<dvec>(n - 1);

    for (int i = 0; i < n; ++i) {
        A(0, i) = -p[i + 1] / p[0];
    }

    cx_vec r = eig_gen(A);

    return cvalarray(r.memptr(), r.n_elem);
}
