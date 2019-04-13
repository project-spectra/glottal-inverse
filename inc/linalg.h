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

#include "constants.h"

using std::array;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using std::map;
using std::pair;

struct VectorFree {
    void operator()(gsl_vector *x) const noexcept;
};

using VecPtr = unique_ptr<gsl_vector, VectorFree>;

using BasisMap = array<VecPtr, basisLength>;
using ConvMap = array<VecPtr, basisLength * basisLength>;


void projForward(gsl_vector *f);
void projBackward(gsl_vector *u);

void projForward(gsl_vector *f, gsl_vector *u);
void projBackward(gsl_vector *u, gsl_vector *f);

gsl_vector *getBasis(size_t k);
gsl_vector *getConvoluted(size_t p, size_t f);

double convolutedBasis(size_t p, size_t f, size_t mu);

void normalize(gsl_vector *f);


extern const VectorFree vecFree;

extern shared_ptr<BasisMap> storedBases;
extern shared_ptr<ConvMap> storedConvs;


#endif // WAVELETS_H
