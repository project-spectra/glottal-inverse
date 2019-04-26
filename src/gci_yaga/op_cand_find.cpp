#include "gci_yaga.h"


void findOpenCandidates(const std::vector<int>& cands, const std::vector<int>& bestCands, std::deque<int>& openCands)
{
    // ~no = ~nco <symdiff> nc

    std::vector<int> inter;

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
