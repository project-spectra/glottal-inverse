#include <cmath>
#include "lf_fitting.h"


double lf_ho(double alpha, int k, const lf_params& p)
{
    return -p.Ee / (std::exp(alpha * p.Te) * std::sin(p.Te * M_PI / p.Tp))
                * std::exp(alpha * k / p.N)
                * std::sin(M_PI / p.Tp * (double) k / p.N);
}

double lf_hr(double epsilon, int k, const lf_params& p)
{
    return -p.Ee / (epsilon * p.Ta)
                * (std::exp(-epsilon * ((double) k / p.N - p.Te))
                    - std::exp(-epsilon * (1. - p.Te)));
}

static constexpr double dx = 0.01;

double lf_Ho(double alpha, int k, const lf_params& p)
{
    return (lf_ho(alpha + dx, k, p) - lf_ho(alpha - dx, k, p)) / (2 * dx);
}

double lf_Hr(double epsilon, int k, const lf_params& p)
{
    return (lf_hr(epsilon + dx, k, p) - lf_hr(epsilon - dx, k, p)) / (2 * dx);
}
