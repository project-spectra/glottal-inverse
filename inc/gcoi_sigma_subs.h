#ifndef GCOI_SIGMA_SUBS_H
#define GCOI_SIGMA_SUBS_H


#include <armadillo>
#include <valarray>

using valarray = std::valarray<double>;


#define SIGMA_nclust (2)

#define SIGMA_fmin (70.)
#define SIGMA_fmax (500.)
#define SIGMA_Tmin (1. / SIGMA_fmax)
#define SIGMA_Tmax (1. / SIGMA_fmin)
#define SIGMA_oqmin (0.1)
#define SIGMA_oqmax (0.9)

#define SIGMA_gwlen (SIGMA_Tmin)


valarray gcoi_sigma_cbrt(const valarray& x);

void gcoi_sigma_swt(const valarray& u, valarray& p);

int gcoi_sigma_gd(const valarray& p, valarray& gd);

void gcoi_sigma_nzcr(const valarray& gd, arma::dvec& zcrs);

template<int dir>
void gcoi_sigma_model(const arma::dvec& gic, const valarray& crmp, const valarray& grdel, arma::dmat& fv);

template<int dir>
int gcoi_sigma_cluster(const arma::dmat& fv, arma::ivec& in);


#endif // GCOI_SIGMA_SUBS_H
