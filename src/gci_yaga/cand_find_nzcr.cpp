#include "gci_yaga.h"
#include "gci_yaga_subroutines.h"


void cand_find_nzcr(const valarray& gamma, candvec& cands)
{
    double cur, next;
    
    next = gamma[0];

    for (int t = 0; t < gamma.size() - 1; ++t) {
        cur = next;
        next = gamma[t + 1];

        // sign change, negative going
        if (cur * next <= 0 && next <= 0) {
            cands.emplace_back(t, true);
        }
    }
}
