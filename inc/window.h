#ifndef WINDOW_H
#define WINDOW_H


#include <gsl/gsl_vector.h>

gsl_vector *hanning(const size_t L);

gsl_vector *blackman(const size_t L);


#endif // WINDOW_H
