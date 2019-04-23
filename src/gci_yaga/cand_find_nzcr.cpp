#include "gci_yaga.h"
#include "gci_yaga_subroutines.h"


void cand_find_nzcr(const valarray& gamma, candvec& cands)
{
    for (size_t n = 0; n < gamma.size() - 1; ++n) {
        if (gamma[n] >= 0 && gamma[n] * gamma[n+1] < 0.) { // negative-going
            cands.emplace_back(n, true);
        }
    } 
}
