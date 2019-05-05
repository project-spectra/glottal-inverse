#include <algorithm>
#include "gci_yaga_subroutines.h"
#include "gci_yaga.h"


void findCandidates(const valarray& gamma, candvec& cands)
{
    // Negative-going zero crossings candidates
    cand_find_nzcr(gamma, cands);

    // Phase slope projection technique candidates
    cand_find_psp(gamma, cands);
    
    // Sort candidates per time
    std::sort(cands.begin(), cands.end());

    // Remove duplicates
    auto end = std::unique(cands.begin(), cands.end(), [](auto& a, auto& b) { return a.first == b.first; });
    cands.erase(end, cands.end());    
}
