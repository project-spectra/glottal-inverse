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

    printIterable(candPairs, "GCI cands");

    if (candPairs.empty()) {
        return;
    }

    selectCandidates(u, gamma, T0mean, candPairs, gci);

    printIterable(candPairs, "GCI selec");

    // Transform the pairs to their time in sample
    std::deque<int> openCands;
    findOpenCandidates(candPairs, gci, openCands);

    printIterable(openCands, "GOI cands");

    selectOpenCandidates(gci, openCands, goi);

    printIterable(goi, "GOI selec");
}
