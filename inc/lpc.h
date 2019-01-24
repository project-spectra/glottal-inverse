#ifndef LPC_H
#define LPC_H


#include <cmath>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_sf_trig.h>
#include <gsl/gsl_blas.h>


gsl_vector *hanning(size_t L);

gsl_vector *lpcCoeffs(gsl_vector *x, size_t M, size_t order);

gsl_vector *lpcResidual(
    gsl_vector *x,
    size_t numSamples,
    size_t L,
    size_t shift,
    size_t order
);


#endif // LPC_H
