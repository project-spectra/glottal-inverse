#ifndef IAIF_H
#define IAIF_H

#include <utility>

#include <gsl/gsl_vector.h>

#include "constants.h"
#include "lpc.h"
#include "filter.h"

std::pair<gsl_vector *, gsl_vector *> computeIAIF(gsl_vector *x);


#endif // IAIF_H
