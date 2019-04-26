#include "window.h"
#include "audio.h"
#include "gci_yaga_subroutines.h"

double cost_ncorr(const valarray& u, const int r, const int p)
{
    // 10ms of correlated speech segments, centered at p and r
    static const int corrLen(10. / 1000. * SAMPLE_RATE);
    static const int corrOff(corrLen / 2);

    valarray u_r(u[std::slice(r - corrOff, corrLen, 1)]);
    valarray u_p(u[std::slice(p - corrOff, corrLen, 1)]);

    double mu_r(u_r.sum() / corrLen);
    double mu_p(u_p.sum() / corrLen);

    valarray var_r(abs(u_r - mu_r));
    valarray var_p(abs(u_p - mu_p));

    double r_stdev(1 / (corrLen - 1) * (var_r * var_r).sum());
    double p_stdev(1 / (corrLen - 1) * (var_p * var_p).sum());

    return 1. / (corrLen * r_stdev * p_stdev) * (u_r * u_p).sum();
}
