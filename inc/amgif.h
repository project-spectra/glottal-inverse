#ifndef AMGIF_H
#define AMGIF_H


#include <utility>
#include <vector>
#include <memory>

#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_spmatrix.h>

#include "constants.h"

using std::pair;
using std::tuple;
using std::vector;
using std::shared_ptr;
using std::array;

// Smart pointer to matrix
using mat_operator = shared_ptr<gsl_spmatrix>;

// Shorthand for template types
using ListCmu = const vector<mat_operator>;
using VecPair = pair<gsl_vector *, gsl_vector *>;
using VecTriplet = tuple<gsl_vector *, gsl_vector *, gsl_vector *>;

// Generate operator L
gsl_spmatrix *computeL();

// Generate operator C
#ifndef PRECOMP
ListCmu
#else
void
#endif
computeC();

void computeSingleC(gsl_spmatrix *C, size_t mu);

// Compute the AM-GIF estimates
VecTriplet computeAMGIF(
        ListCmu& C,
        gsl_vector *me,
        gsl_vector *pe,
        const mat_operator& L,
        const double alpha,
        const double beta,
        const double tau,
        const double eps
); 


#endif // AMGIF_H
