#ifndef INTERP_SAMPLE_H
#define INTERP_SAMPLE_H


#include <cmath>

#include <gsl/gsl_interp.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>

#include "constants.h"


double interp_sample_eval(double t, void *p);

void *interp_sample(gsl_vector *y, bool periodic);


#endif // INTERP_SAMPLE_H
