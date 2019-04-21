#ifndef GCI_YAGA_H
#define GCI_YAGA_H


#include <utility>
#include <valarray>
#include <deque>
#include <vector>

#include <gsl/gsl_vector.h>

using valarray = std::valarray<double>;

// (n, is_zcr)
using cand = std::pair<size_t, bool>;
using candvec = std::deque<cand>;

void gci_yaga(gsl_vector *dg);

void computeSWT(gsl_vector *u, valarray& pm);
void computeGD(const valarray& pm, valarray& gamma);

void findCandidates(const valarray& gamma, candvec& cands);
void selectCandidates(const gsl_vector *u, const valarray& gamma, candvec& cands, std::vector<size_t>& bestCands);

void swt_app_det_init(const gsl_vector *u, const size_t J, const size_t j1, std::vector<valarray>& g, std::vector<valarray>& h);

void cand_find_nzcr(const valarray& gamma, candvec& cands);
void cand_find_psp(const valarray& gamma, candvec& cands);

void cand_select_precalc_norms(const gsl_vector *u, const candvec& cands, valarray& norms, double& maxNorm);

double cost_eval(const valarray& lambda, const valarray& costs);

void cost_calc(const gsl_vector *u, const valarray& gamma, const valarray& norms, const double maxNorm, const cand& r_cand, const cand& q_cand, const cand& p_cand, valarray& cost);

double cost_khi(const double mu, const double sigma, const double y);
double cost_ncorr(const gsl_vector *u, const size_t r, const size_t p);
double cost_FN(const gsl_vector *u, const size_t r);
double cost_ssqe(const valarray& gamma, const size_t r);
double cost_CP(const valarray& norms, const double maxNorm, const size_t r);

#endif // GCI_YAGA_H
