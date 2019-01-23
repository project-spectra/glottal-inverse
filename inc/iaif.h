#ifndef IAIF_H
#define IAIF_H


#include <gsl/gsl_vector.h>

#include "constants.h"
#include "lpc.h"
#include "filter.h"

gsl_vector *computeIAIF(gsl_vector *x, size_t M);


#endif // IAIF_H
