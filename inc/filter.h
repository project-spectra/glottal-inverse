#ifndef FILTER_H
#define FILTER_H


#include <gsl/gsl_vector.h>

#include <utility>


void filter_fir(gsl_vector *b, gsl_vector *x, gsl_vector *res);

void filter_iir(gsl_vector *b, gsl_vector *a, gsl_vector *x, gsl_vector *res);

void filter_hpf(gsl_vector *x, const double fc);


#endif // FILTER_H
