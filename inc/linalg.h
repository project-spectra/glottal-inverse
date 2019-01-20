#ifndef BASIS_H
#define BASIS_H

#include <stddef.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_integration.h>


// Evaluate wavelet basis (j > J, l is ignored, scaling function)
int wavelet_basis(size_t j, size_t l, double t);

// Integration
double integrate(const gsl_function *f, double a, double b);
void integrate_dispose();

// Projection and convolution
double projection(const gsl_function *f, const gsl_function *g);
double convolute(const gsl_function *f, const gsl_function *g, double t);


#endif // WAVELETS_H
