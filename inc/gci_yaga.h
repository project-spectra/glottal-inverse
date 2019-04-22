#ifndef GCI_YAGA_H
#define GCI_YAGA_H


#include <utility>
#include <valarray>
#include <deque>
#include <vector>

#include <gsl/gsl_vector.h>

using valarray = std::valarray<double>;
using cand = std::pair<size_t, bool>;
using candvec = std::deque<cand>;

void gci_yaga(gsl_vector *dg);

void computeSWT(gsl_vector *u, valarray& pm);
void computeGD(const valarray& pm, valarray& gamma);

void findCandidates(const valarray& gamma, candvec& cands);
void selectCandidates(const gsl_vector *u, const valarray& gamma, candvec& cands, std::vector<size_t>& bestCands);

void findOpenCandidates(const std::vector<size_t>& cands, const std::vector<size_t>& bestCands, std::deque<size_t>& openCands);
void selectOpenCandidates(const std::vector<size_t>& closed, const std::deque<size_t>& open, std::vector<size_t>& bestOpen);

#endif // GCI_YAGA_H
