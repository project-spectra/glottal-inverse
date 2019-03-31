#ifndef AMGIF_H
#define AMGIF_H


#include <utility>
#include <vector>
#include <memory>

#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_monte_miser.h>

#include "constants.h"

using std::pair;
using std::vector;
using std::shared_ptr;

// Smart pointer to matrix
using mat_operator = shared_ptr<gsl_matrix>;

// Shorthand for template types
using ListCmu = const vector<mat_operator>;
using VecPair = pair<gsl_vector *, gsl_vector *>;

// Generate operator L
mat_operator computeL();

// Generate operator C
ListCmu computeC();

// Compute the AM-GIF estimates
VecPair computeAMGIF(
        ListCmu& C,
        gsl_vector *me,
        gsl_vector *pe,
        mat_operator& L,
        double alpha,
        double beta,
        double tau,
        double eps
); 


#endif // AMGIF_H
