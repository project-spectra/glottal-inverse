#include "gci_yaga.h"


void gci_yaga(gsl_vector *u)
{
    valarray pm;
    computeSWT(u, pm);

    valarray gamma;
    computeGD(pm, gamma);

    candvec cands;
    findCandidates(gamma, cands);

    selectCandidates(u, gamma, cands);

}
