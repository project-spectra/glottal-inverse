#ifndef GCI_SEDREAMS_H
#define GCI_SEDREAMS_H

#include <deque>
#include <vector>

#include <gsl/gsl_vector.h>

std::vector<size_t> gci_sedreams(gsl_vector *signal, const double fs, const double f0mean);

std::deque<size_t> findPeaks(gsl_vector *signal, double sign);

int median(std::vector<int>& x);

#endif // GCI_SEDREAMS_H
