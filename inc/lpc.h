#ifndef LPC_H
#define LPC_H


#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

double lpcCoeffs(double *lpc, gsl_vector *x, size_t order);

void lpcResidual(gsl_matrix *lpc, gsl_vector *res, gsl_vector *x, size_t L, size_t shift, size_t order);


#endif // LPC_H
