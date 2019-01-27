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

#include "constants.h"

using std::vector;
using std::unique_ptr;
using std::shared_ptr;

// Basis length
constexpr size_t basisLength() {
    // There are Sum[0..J] 2^j wavelets.
    // That's 2^(J+1) - 1 elements.
    // Add one for the constant scaling function.
    return 2 << (J + 1);
}

// Wavelet basis
void constructBasis();
gsl_vector *basis(size_t index);

// Integration shorthand
double integrate(gsl_vector *f);

// Projection
struct projection_params {
    const gsl_function *f;
    const gsl_function *g;
};
double projection(gsl_vector *f, gsl_vector *g);

// Convolution
struct convolute_params {
    const gsl_function *f;
    const gsl_function *g;
    double t;
};
gsl_vector *convolute(gsl_vector *f, gsl_vector *g);

// Coordinate system transforms
gsl_vector *coords(gsl_vector *f);
gsl_vector *uncoords(gsl_vector *u);

// Normalize vector as a signal (max amplitude)
void normalize(gsl_vector *x);

#endif // WAVELETS_H
