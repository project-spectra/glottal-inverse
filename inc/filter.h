#ifndef FILTER_H
#define FILTER_H


#include <gsl/gsl_vector.h>

gsl_vector *filter_fir(gsl_vector *b, gsl_vector *x);

gsl_vector *filter_iir(gsl_vector *b, gsl_vector *a, gsl_vector *x);


#endif // FILTER_H
