#ifndef DERIVATIVE_H
#define DERIVATIVE_H


#include <memory>

#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_interp.h>

#include "constants.h"


void deriv_discrete(gsl_vector *df, gsl_vector *f, double step);


#endif // DERIVATIVE_H
