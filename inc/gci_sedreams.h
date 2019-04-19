#ifndef GCI_SEDREAMS_H
#define GCI_SEDREAMS_H

#include <cmath>
#include <deque>
#include <vector>

#include <gsl/gsl_vector.h>

#include "constants.h"
#include "filter.h"
#include "window.h"
#include "lpc.h"
#include "normalize.h"

std::vector<size_t> gci_sedreams(gsl_vector *signal, const double fs, const double f0mean);

std::deque<size_t> findPeaks(gsl_vector *signal, double sign);

double median(std::vector<size_t>& x);

#endif // GCI_SEDREAMS_H
