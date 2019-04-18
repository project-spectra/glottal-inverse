#ifndef GLOTTAL_PHASES_H
#define GLOTTAL_PHASES_H


#include <array>
#include <vector>
#include <cstring>

#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_vector_complex.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_fft_complex.h>

#include "constants.h"


void estimatePhases(gsl_vector *dEGG, double T0, double *Oq, double *Cq);


#endif // GLOTTAL_PHASES_H
