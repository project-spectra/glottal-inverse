#ifndef GCI_YAGA_H
#define GCI_YAGA_H


#include <utility>
#include <valarray>
#include <deque>
#include <vector>

using valarray = std::valarray<double>;
using cand = std::pair<size_t, bool>;
using candvec = std::deque<cand>;

void gci_yaga(const valarray& dg);

void computeSWT(const valarray& u, valarray& pm);
void computeGD(const valarray& pm, valarray& gamma);

void findCandidates(const valarray& gamma, candvec& cands);
void selectCandidates(const valarray& u, const valarray& gamma, candvec& cands, std::vector<size_t>& bestCands);

void findOpenCandidates(const std::vector<size_t>& cands, const std::vector<size_t>& bestCands, std::deque<size_t>& openCands);
void selectOpenCandidates(const std::vector<size_t>& closed, const std::deque<size_t>& open, std::vector<size_t>& bestOpen);

#endif // GCI_YAGA_H
