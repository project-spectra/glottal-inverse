#include "gci_yaga.h"

 
static constexpr double constLambda[] = {
    0.8, 0.6, 0.4, 0.3, 0.1, 0.5
};

static constexpr double xhi = 0.1;

void selectCandidates(const gsl_vector *u, const valarray& gamma, candvec& cands)
{
    const size_t Ncand(cands.size());

    std::vector<valarray> costs(Ncand);
    valarray lambda(constLambda, 6);

    size_t r;
    
    // initialize cost vectors
    for (r = 0; r < Ncand; ++r) {
        cost_init(u, gamma, cands[r], costs[r]);
    }
}
