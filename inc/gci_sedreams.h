#ifndef GCI_SEDREAMS_H
#define GCI_SEDREAMS_H

#include <deque>
#include <utility>
#include <valarray>
#include <vector>

using valarray = std::valarray<double>;

std::vector<size_t> gci_sedreams(const valarray& signal, const double fs, const double f0mean);

std::deque<size_t> findPeaks(const valarray& signal, double sign);

double median(std::vector<double>& x);

#endif // GCI_SEDREAMS_H
