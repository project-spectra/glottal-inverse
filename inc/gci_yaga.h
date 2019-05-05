#ifndef GCI_YAGA_H
#define GCI_YAGA_H


#include <utility>
#include <valarray>
#include <deque>
#include <vector>

using valarray = std::valarray<double>;
using cand = std::pair<int, bool>;
using candvec = std::deque<cand>;

void gci_yaga(const valarray& dg, const double TOmean, std::vector<int>& gci, std::vector<int>& goi);

void computeSWT(const valarray& u, valarray& pm);
void computeGD(const valarray& pm, valarray& gamma);

void findCandidates(const valarray& gamma, candvec& cands);
void selectCandidates(const valarray& u, const valarray& gamma, const double T0mean, candvec& cands, std::vector<int>& bestCands);

void findOpenCandidates(const candvec& cands, const std::vector<int>& bestCands, std::deque<int>& openCands);
void selectOpenCandidates(const std::vector<int>& closed, const std::deque<int>& open, std::vector<int>& bestOpen);

#endif // GCI_YAGA_H
