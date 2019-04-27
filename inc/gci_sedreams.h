#ifndef GCI_SEDREAMS_H
#define GCI_SEDREAMS_H

#include <deque>
#include <utility>
#include <valarray>
#include <vector>

using valarray = std::valarray<double>;

void gci_sedreams(const valarray& signal, const double T0mean, std::vector<int>& gci, std::vector<int>& goi);

std::deque<int> findPeaks(const valarray& signal, double sign);

void findZeroCrossings(const valarray& signal, std::vector<int>& pos, std::vector<int>& neg);

double median(std::vector<double>& x);

#endif // GCI_SEDREAMS_H
