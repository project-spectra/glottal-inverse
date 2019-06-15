#include <algorithm>
#include <climits>
#include "gnuplot.h"
#include "normalize.h"
#include "print_iter.h"
#include "gci_yaga.h"
#include "gci_sedreams.h"


void gci_yaga(const valarray& u, const double T0mean, std::vector<int>& gci, std::vector<int>& goi)
{
    valarray p;
    computeSWT(u, p);

    valarray gamma;
    computeGD(p, gamma);
   
    writePlotData(p, "p.dat");
    writePlotData(gamma, "g.dat");

    candvec candPairs;
    findCandidates(gamma, candPairs);

    std::vector<int> cands;
    std::transform(candPairs.begin(), candPairs.end(), std::back_inserter(cands), [](auto& c) { return c.first; });
    std::cout << "GCI cands: " << cands << std::endl;

    if (candPairs.empty()) {
        return;
    }

    selectCandidates(u, gamma, T0mean, candPairs, gci);

    std::cout << "GCI selected: " << gci << std::endl;

    // Transform the pairs to their time in sample
    std::deque<int> openCands;
    findOpenCandidates(candPairs, gci, openCands);

    std::cout << "GOI cands: " << openCands << std::endl;

    selectOpenCandidates(gci, openCands, goi);

    std::cout << "GOI selected: " << goi << std::endl;
}
