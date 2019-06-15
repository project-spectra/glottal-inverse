#include <chrono>
#include "lf_fitting.h"

lf_alpha_params lf_fit_alpha_init(std::default_random_engine& r, double a0, double P0, double Ro)
{
    lf_alpha_params s = {
        .ak_1 = a0,
        .Pk_1 = P0,
        .a0 = a0,
        .Ro = Ro,
        .rdgen = r,
        .v = std::normal_distribution<double>(0.0, Ro)
    };

    return s;
}

void lf_fit_alpha_update(int k, const lf_params& p, lf_alpha_params& s)
{
    double Ek = lf_ho(s.a0, k, p) + s.v(s.rdgen);

    double ak_ = s.ak_1;
    double Pk_ = s.Pk_1;

    double Hk_ = lf_Ho(ak_, k, p);
    double hk_ = lf_ho(ak_, k, p);

    double Kk = Pk_ * Hk_ / (Hk_ * Pk_ * Hk_ + s.Ro);
    double ak = ak_ + Kk * (Ek - hk_);
    double Pk = (1 - Kk * Hk_) * Pk_;

    s.ak_1 = ak;
    s.Pk_1 = Pk;
}
