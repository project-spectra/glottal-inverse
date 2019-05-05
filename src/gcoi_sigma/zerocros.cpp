#include "gcoi_sigma_subs.h"

void gcoi_sigma_nzcr(const valarray& gd, arma::ivec& zcrs, int toff)
{
    std::vector<arma::sword> zcrsList;

    for (int n = 0; n < gd.size() - 1; ++n) {
        if (gd[n] > 0. && gd[n] * gd[n+1] < 0.) { // negative-going
            zcrsList.emplace_back(n + toff);
        }
    }

    zcrs = zcrsList;
}
