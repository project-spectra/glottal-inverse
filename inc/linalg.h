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

// Basis length
constexpr size_t basis_length() {
    // There are Sum[0..J] 2^j wavelets.
    // That's 2^(J+1) - 1 elements.
    // Add one for the constant scaling function.
    return 2 << (J + 1);
}

void convolute(gsl_vector *f, gsl_vector *g, gsl_vector *w);

void coords(double f[], double u[]);
void uncoords(double u[], double f[]);

void convolute(gsl_vector *f, gsl_vector *g, gsl_vector *w);

#endif // WAVELETS_H
