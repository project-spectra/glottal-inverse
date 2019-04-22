#ifndef GCI_YAGA_SUBROUTINES_H
#define GCI_YAGA_SUBROUTINES_H


#include <deque>
#include <utility>
#include <valarray>

using valarray = std::valarray<double>;
using cand = std::pair<size_t, bool>;
using candvec = std::deque<cand>;


void cand_find_nzcr(const valarray& gamma, candvec& cands);
void cand_find_psp(const valarray& gamma, candvec& cands);

void cand_select_precalc_norms(const valarray& u, const candvec& cands, valarray& norms, double& maxNorm);

double cost_eval(const valarray& lambda, const valarray& costs);

void cost_calc(const valarray& u, const valarray& gamma, const valarray& norms, const double maxNorm, const cand& r_cand, const cand& q_cand, const cand& p_cand, valarray& cost);

double cost_khi(const double mu, const double sigma, const double y);
double cost_ncorr(const valarray& u, const size_t r, const size_t p);
double cost_FN(const valarray& u, const size_t r);
double cost_ssqe(const valarray& gamma, const size_t r);
double cost_CP(const valarray& norms, const double maxNorm, const size_t r);


#endif // GCI_YAGA_ROUTINES_H
