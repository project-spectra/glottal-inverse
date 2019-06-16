#ifndef LF_FITTING_H
#define LF_FITTING_H


#include <random>
#include <valarray>

#define M_PI 3.14159265358979323846

using valarray = std::valarray<double>;


struct lf_params {
    double Ee, Tp, Te, Ta, N;
};

struct lf_alpha_params {
    double ak_1, Pk_1;
    double a0, Ro;
    std::default_random_engine rdgen;
    std::normal_distribution<double> v;
};

struct lf_epsilon_params {
    double ek_1, Pk_1;
    double e0, Ro;
    std::default_random_engine rdgen;
    std::normal_distribution<double> v;
};

double lf_ho(double alpha, int k, const lf_params& p);
double lf_hr(double epsilon, int k, const lf_params& p);

double lf_Ho(double alpha, int k, const lf_params& p);
double lf_Hr(double epsilon, int k, const lf_params& p);

lf_alpha_params lf_fit_alpha_init(std::default_random_engine& r, double a0, double P0 = 1.0, double Ro = 0.01);
void lf_fit_alpha_update(int k, const lf_params& p, lf_alpha_params& s);

lf_epsilon_params lf_fit_epsilon_init(std::default_random_engine& r, double e0, double P0 = 1.0, double Ro = 0.01);
void lf_fit_epsilon_update(int k, const lf_params& p, lf_epsilon_params& s);

double lf_mmse(const valarray& r0, const valarray& r);

bool lf_fit_period(const valarray& u, int te, double T0, lf_params& out);

void lf_fit(const valarray& gfd, const std::vector<int>& GCIs, double T0, std::vector<lf_params>& out);

#endif // LF_FITTING_H
