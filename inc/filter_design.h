#ifndef FILTER_DESIGN_H
#define FILTER_DESIGN_H


#include <armadillo>
#include <array>
#include <valarray>

#define M_PI 3.14159265358979323846

using valarray = std::valarray<double>;

struct filter_zpk {
    arma::cx_vec z, p;
    arma::cx_double k;
};

struct filter_tf {
    arma::cx_vec b, a;
};

struct filter_ss {
    arma::cx_mat A;
    arma::cx_vec B;
    arma::cx_rowvec C;
    arma::cx_double D;
};

enum filter_type {
    LOWPASS, HIGHPASS
};

void convert_zpk2tf(const filter_zpk& src, filter_tf& dst);
void convert_tf2ss(const filter_tf& src, filter_ss& dst);
void convert_ss2tf(const filter_ss& src, filter_tf& dst);
void convert_zpk2ss(const filter_zpk& src, filter_ss& dst);

void convert_bilinear(const filter_ss& src, filter_ss& dst);
void convert_bilinear(const filter_ss& src, double fp, filter_ss& dst);

void transform_lp2lp(const filter_ss& src, double Wo, filter_ss& dst);
void transform_lp2hp(const filter_ss& src, double Wo, filter_ss& dst);

void filter_butter_ap(int N, filter_zpk& dst);
std::array<valarray, 2> filter_butter(int N, double fc, filter_type fType = LOWPASS);

#endif // FILTER_DESIGN_H
