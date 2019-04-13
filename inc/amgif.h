#ifndef AMGIF_H
#define AMGIF_H


#include <memory>
#include <utility>
#include <vector>

#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_splinalg.h>
#include <gsl/gsl_spmatrix.h>

#include "constants.h"

using std::array;
using std::pair;
using std::shared_ptr;
using std::tuple;
using std::vector;

// Smart pointer to matrix
using mat_operator = shared_ptr<gsl_spmatrix>;

// Shorthand for template types
using ListCmu = const vector<mat_operator>;
using VecArr = array<double, basisLength>;
using VecTriplet = tuple<gsl_vector *, gsl_vector *, gsl_vector *>;

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
