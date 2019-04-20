#ifndef PITCH_H
#define PITCH_H


#include <gsl/gsl_vector.h>

#define DECL_PITCH(name)  bool pitch_ ## name (gsl_vector *frame, double *f0res, double *T0res)


DECL_PITCH(AMDF);


#endif // PITCH_H
