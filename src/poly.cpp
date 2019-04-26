#include "poly.h"

using Eigen::VectorXcd;


VectorXcd poly(const VectorXcd& zeros)
{
    VectorXcd a(zeros.size() + 1);

    a[0] = 1.;

    for (size_t j = 0; j < zeros.size(); ++j) {
        a.head(j) -= zeros[j] * a.head(j);
    }

    return a;
}

