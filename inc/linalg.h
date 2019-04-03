#ifndef BASIS_H
#define BASIS_H


#include <cstddef>
#include <cmath>
#include <map>
#include <memory>

#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_wavelet.h>

#include "constants.h"

using std::shared_ptr;
using std::map;
using std::pair;

using IndPair = pair<size_t, size_t>;
using PtrVector = shared_ptr<gsl_vector>;
using BasisMap = map<size_t, PtrVector>;
using ConvMap = map<IndPair, PtrVector>;

void coords(double f[], double u[]);
void uncoords(double u[], double f[]);

void normalize(gsl_vector *f);

PtrVector convoluteBasis(size_t p, size_t f);

#endif // WAVELETS_H
