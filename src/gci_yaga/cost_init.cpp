#include "gci_yaga.h"


void cost_init(const gsl_vector *u, const valarray& gamma, const cand& r_cand, valarray& cost)
{
    cost_calc(u, gamma, r_cand, r_cand, r_cand, cost);
}
