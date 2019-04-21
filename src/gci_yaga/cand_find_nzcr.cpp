#include "gci_yaga.h"


void cand_find_nzcr(const valarray& gamma, candvec& cands)
{
    valarray sign(gamma * gamma.shift(1));

    auto nzcr(sign < 0. && gamma >= 0.);

    for (size_t n = 0; n < nzcr.size(); ++n) {
        if (nzcr[n]) {
            cands.emplace_back(n, true);
        }
    } 
}
