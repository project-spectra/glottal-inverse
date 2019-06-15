#include <chrono>
#include "lf_fitting.h"

lf_epsilon_params lf_fit_epsilon_init(std::default_random_engine& r, double e0, double P0, double Ro)
{
    lf_epsilon_params s = {
        .ek_1 = e0,
        .Pk_1 = P0,
        .e0 = e0,
        .Ro = Ro,
        .rdgen = r,
        .v = std::normal_distribution<double>(0.0, Ro)
    };

    return s;
}

void lf_fit_epsilon_update(int k, const lf_params& p, lf_epsilon_params& s)
{
    double Ek = lf_hr(s.e0, k, p) + s.v(s.rdgen);

    double ek_ = s.ek_1;
    double Pk_ = s.Pk_1;

    double Hk_ = lf_Hr(ek_, k, p);
    double hk_ = lf_hr(ek_, k, p);

    double Kk = Pk_ * Hk_ / (Hk_ * Pk_ * Hk_ + s.Ro);
    double ek = ek_ + Kk * (Ek - hk_);

    double Pk = (1 - Kk * Hk_) * Pk_;

    s.ek_1 = ek;
    s.Pk_1 = Pk;
}
