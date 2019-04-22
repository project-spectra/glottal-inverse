#include <algorithm>
#include "print_iter.h"
#include "gci_yaga.h"


void gci_yaga(gsl_vector *u)
{
    valarray pm;
    computeSWT(u, pm);

    valarray gamma;
    computeGD(pm, gamma);

    candvec candPairs;
    findCandidates(gamma, candPairs);

    std::vector<size_t> bestCands;
    selectCandidates(u, gamma, candPairs, bestCands);

    // Transform the pairs to their time in samples
    std::vector<size_t> cands(candPairs.size());
    std::transform(candPairs.begin(), candPairs.end(), cands.begin(),
                    [] (const auto& c) { return c.first; });

    // Remove duplicate entries
    auto last = std::unique(cands.begin(), cands.end());
    cands.erase(last, cands.end());

    std::deque<size_t> openCands;
    findOpenCandidates(cands, bestCands, openCands);

    std::vector<size_t> bestOpenCands;
    selectOpenCandidates(bestCands, openCands, bestOpenCands);

    printIterable(bestCands, "GCIs");
    printIterable(bestOpenCands, "GOIs");
}
