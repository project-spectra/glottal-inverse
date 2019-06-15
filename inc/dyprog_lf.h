#ifndef DYPROG_LF_H
#define DYPROG_LF_H


#include <armadillo>
#include <valarray>
#include <vector>

using valarray = std::valarray<double>;

void dyprog_lf(const valarray& dg, std::vector<int>& GCIs);


void correct_GCI_glot(const valarray& dg, std::vector<int>& GCIs);

arma::dvec get_spec_stat(const valarray& dg, std::vector<int>& GCIs);

double distitar(const valarray& ar1, const valarray& ar2);

#endif // DYPROG_LF_H
