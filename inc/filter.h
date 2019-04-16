#ifndef FILTER_H
#define FILTER_H


#include <gsl/gsl_vector.h>

#include <utility>


void filter_fir(gsl_vector *b, gsl_vector *x, gsl_vector *y);

void filter_iir(gsl_vector *b, gsl_vector *a, gsl_vector *x);


std::pair<gsl_vector *, gsl_vector *> create_hp_butterworth(const size_t N, const double fc);


#endif // FILTER_H
