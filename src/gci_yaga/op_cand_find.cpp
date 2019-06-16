#include <algorithm>

#include "print_iter.h"
#include "gci_yaga.h"


void findOpenCandidates(const candvec& cands, const std::vector<int>& bestCands, std::deque<int>& openCands)
{
    // ~no = ~nco <symdiff> nc

    std::vector<int> candTimes(cands.size());
    std::transform(cands.begin(), cands.end(), candTimes.begin(),
                    [] (const auto& c) { return c.first; });

    std::sort(candTimes.begin(), candTimes.end());
  
    std::set_symmetric_difference(
            candTimes.begin(), candTimes.end(),
            bestCands.begin(), bestCands.end(),
            std::back_inserter(openCands)
    );
}
