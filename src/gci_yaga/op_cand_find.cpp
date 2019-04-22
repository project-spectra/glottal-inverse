#include "gci_yaga.h"


void findOpenCandidates(const std::vector<size_t>& cands, const std::vector<size_t>& bestCands, std::deque<size_t>& openCands)
{
    // ~no = ~nco <symdiff> nc

    std::vector<size_t> inter;

    std::set_intersection(
            cands.begin(), cands.end(),
            bestCands.begin(), bestCands.end(),
            std::back_inserter(inter)
    );
    
    std::set_difference(
            cands.begin(), cands.end(),
            inter.begin(), inter.end(),
            std::back_inserter(openCands)
    );
}
