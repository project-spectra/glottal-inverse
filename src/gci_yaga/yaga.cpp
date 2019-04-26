#include <algorithm>
#include "gnuplot.h"
#include "normalize.h"
#include "print_iter.h"
#include "gci_yaga.h"


void gci_yaga(const valarray& dg, std::vector<int>& gci, std::vector<int>& goi)
{
    // pre-emphasize u by computing its Teager operator
    valarray u = dg;// * (dg * dg - dg.cshift(1) * dg.cshift(-1));

    valarray pm;
    computeSWT(u, pm);

    writePlotData(pm, "yaga_pm.dat");

    valarray gamma;
    computeGD(pm, gamma);

    writePlotData(gamma, "yaga_gamma.dat");
    
    candvec candPairs;
    findCandidates(gamma, candPairs);

    if (candPairs.empty()) {
        return;
    }

    selectCandidates(u, gamma, candPairs, gci);

    // Transform the pairs to their time in samples
    std::vector<int> cands(candPairs.size());
    std::transform(candPairs.begin(), candPairs.end(), cands.begin(),
                    [] (const auto& c) { return c.first; });

    // Remove duplicate entries
    auto last = std::unique(cands.begin(), cands.end());
    cands.erase(last, cands.end());

    std::deque<int> openCands;
    findOpenCandidates(cands, gci, openCands);

    selectOpenCandidates(gci, openCands, goi);
}
