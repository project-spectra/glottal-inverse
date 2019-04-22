#include "gci_yaga.h"
#include "gci_yaga_subroutines.h"


void cand_find_nzcr(const valarray& gamma, candvec& cands)
{
    valarray sign(gamma * gamma.shift(1));

    for (size_t n = 0; n < sign.size(); ++n) {
        if (sign[n] < 0. && gamma[n] >= 0) { // negative-going
            cands.emplace_back(n, true);
        }
    } 
}
