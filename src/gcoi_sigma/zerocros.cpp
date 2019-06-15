#include "gcoi_sigma_subs.h"

void gcoi_sigma_nzcr(const valarray& gd, arma::dvec& zcrs)
{
    std::vector<double> zcrsList;

    for (int n = 0; n < gd.size() - 1; ++n) {
        if (gd[n] >= 0. && gd[n + 1] < 0.) {
            zcrsList.push_back(n - gd[n] / (gd[n + 1] - gd[n]));
        }
    }

    zcrs = zcrsList;
}
