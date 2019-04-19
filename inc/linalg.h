#ifndef BASIS_H
#define BASIS_H


#include <algorithm>
#include <cstddef>
#include <cmath>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_spblas.h>
#include <gsl/gsl_spmatrix.h>
#include <gsl/gsl_wavelet.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_vector_complex.h>

#include "constants.h"
#include "vector.h"

using std::array;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using std::map;
using std::pair;

using BasisMap = Vector[]; // length
using ConvMap = Vector[]; // length * length


void projForward(gsl_vector *f);
void projBackward(gsl_vector *u);

void projForward(gsl_vector *f, gsl_vector *u);
void projBackward(gsl_vector *u, gsl_vector *f);

const Vector& getBasis(size_t k);
const Vector& getConvoluted(size_t p, size_t f);

double convolutedBasis(size_t p, size_t f, size_t mu);


void normalize(gsl_vector *f, double a);
void normalize_complex(gsl_vector_complex *f, double a);



extern unique_ptr<BasisMap> storedBases;
extern unique_ptr<ConvMap> storedConvs;


#endif // WAVELETS_H
