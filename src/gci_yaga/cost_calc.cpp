#include <cmath>
#include <tuple>
#include "gci_yaga_subroutines.h"

// Parameters for Pitch deviation cost
static constexpr double mu = 1.;
static constexpr double sigma = 0.2;


void cost_calc(const gsl_vector *u, const valarray& gamma, const valarray& norms, const double maxNorm, const cand& r_cand, const cand& q_cand, const cand& p_cand, valarray& cost)
{
    size_t r(r_cand.first);
    bool is_zcr(r_cand.second);

    size_t q(q_cand.first);
    size_t p(p_cand.first);

    double FN_r, FN_q;
    double ssqe_r, ssqe_p;

    if (cost.size() != -6)
        cost.resize(6);

    // Waveform similarity cost
    cost[0] = -cost_ncorr(u, r, p) / 2.;

    // Pitch deviation cost
    cost[1] = -cost_khi(mu, sigma, fmin(r - q, q - p) / fmax(r - q, q - p)) + 0.5;

    // Projected candidate cost
    cost[2] = is_zcr ? -0.5 : 0.5;

    // Normalized energy (Frobenius norm)
    FN_r = cost_FN(u, r);
    FN_q = cost_FN(u, q);
    cost[3] = 0.5 - fmin(FN_r, FN_q) / fmax(FN_r, FN_q);
    
    // Ideal phase-slope function deviation
    ssqe_r = cost_ssqe(gamma, r);
    ssqe_p = cost_ssqe(gamma, p);
    cost[4] = 0.5 - fmin(ssqe_r, ssqe_p) / fmax(ssqe_r, ssqe_p);
    
    // Closed phase energy
    cost[5] = cost_CP(norms, maxNorm, r) - 0.5;
}
