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
constexpr size_t basis_length() {
    // There are Sum[0..J] 2^j wavelets.
    // That's 2^(J+1) - 1 elements.
    // Add one for the constant scaling function.
    return 2 << (J + 1);
}

// Wavelet basis
size_t basis_index(size_t j, size_t l);
double basis_eval(double t, void *params);

// Integration shorthand
double integrate(const gsl_function *f, double a, double b);
void integrate_dispose();

// Projection
struct projection_params {
    const gsl_function *f;
    const gsl_function *g;
};
double projection(gsl_function *f, gsl_function *g);

// Convolution
struct convolute_params {
    const gsl_function *f;
    const gsl_function *g;
    double t;
};
double convolute(double t, void *p);

// Coordinate system transforms
gsl_vector *coords(gsl_function *f);
double coords_eval(double t, void *p);


#endif // WAVELETS_H
