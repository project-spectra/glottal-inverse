#ifndef BASIS_H
#define BASIS_H


#include <cstddef>
#include <cmath>
#include <vector>
#include <memory>

#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_wavelet.h>

#include "constants.h"

using std::vector;
using std::unique_ptr;
using std::shared_ptr;


// Wavelet basis
gsl_vector *coords(gsl_vector *f);
gsl_vector *uncoords(gsl_vector *u);
gsl_vector *basis(size_t index);

// Normalize vector as a signal (max amplitude)
void normalize(gsl_vector *x);

// Convolution
gsl_vector *convolute(gsl_vector *f, gsl_vector *g);


#endif // WAVELETS_H
