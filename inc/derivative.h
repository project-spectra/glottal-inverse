#ifndef DERIVATIVE_H
#define DERIVATIVE_H


#include <gsl/gsl_vector.h>

void deriv_discrete(gsl_vector *df, gsl_vector *f, double step);


#endif // DERIVATIVE_H
