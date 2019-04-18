#ifndef IAIF_H
#define IAIF_H

#include <utility>
#include <tuple>

#include <gsl/gsl_vector.h>

#include "constants.h"
#include "lpc.h"
#include "filter.h"

void computeIAIF(gsl_vector *g, gsl_vector *dg, gsl_vector *x);


#endif // IAIF_H
