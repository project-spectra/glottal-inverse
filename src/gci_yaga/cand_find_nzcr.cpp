#include "gci_yaga.h"
#include "gci_yaga_subroutines.h"


void cand_find_nzcr(const valarray& gamma, candvec& cands)
{
    valarray sign(gamma * gamma.shift(1));

    std::valarray<bool> zcr(sign < 0.); 

    for (size_t n = 0; n < zcr.size(); ++n) {
        if (zcr[n] && gamma[n] >= 0) { // negative-going
            cands.emplace_back(n, true);
        }
    } 
}
