#include "print_iter.h"
#include "gci_yaga.h"


void gci_yaga(gsl_vector *u)
{
    valarray pm;
    computeSWT(u, pm);

    valarray gamma;
    computeGD(pm, gamma);

    candvec cands;
    findCandidates(gamma, cands);

    std::vector<size_t> bestCands;
    selectCandidates(u, gamma, cands, bestCands);

    printIterable(bestCands, "bestCands");
}
