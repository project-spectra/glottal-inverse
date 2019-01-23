#ifndef FILTER_H
#define FILTER_H


#include <gsl/gsl_vector.h>

gsl_vector *filter_fir(gsl_vector *b, size_t N, gsl_vector *x, size_t M);

gsl_vector *filter_iir(gsl_vector *b, size_t Nb, gsl_vector *a, size_t Na, gsl_vector *x, size_t M);


#endif // FILTER_H
