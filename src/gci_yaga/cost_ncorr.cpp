#include "constants.h"
#include "gci_yaga.h"

// 10ms of correlated speech segments, centered at p and r
static constexpr size_t corrLen = 10. / 1000. * SAMPLE_RATE;

// Speech segment offset
static constexpr size_t corrOff = corrLen / 2;

double cost_ncorr(const gsl_vector *u, const size_t r, const size_t p)
{
    valarray u_r(u->data + r - corrOff, corrLen);
    valarray u_p(u->data + p - corrOff, corrLen);

    double mu_r(u_r.sum() / corrLen);
    double mu_p(u_p.sum() / corrLen);

    auto var_r(abs(u_r - mu_r));
    auto var_p(abs(u_p - mu_p));

    double r_stdev(1 / (corrLen - 1) * (var_r * var_r).sum());
    double p_stdev(1 / (corrLen - 1) * (var_p * var_p).sum());

    return 1. / (corrLen * r_stdev * p_stdev) * (u_r * u_p).sum();
}
