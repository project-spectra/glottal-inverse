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
using std::unique_ptr;

// Smart pointer to matrix
using mat_operator = unique_ptr<gsl_matrix, void(*)(gsl_matrix *)>;

// Generate operator L
mat_operator computeL();

// Generate operator C
vector<mat_operator> computeC();

// Compute the AM-GIF estimates
pair<gsl_vector *, gsl_vector *> computeAMGIF(
        vector<mat_operator>& C,
        gsl_function *me,
        gsl_function *pe,
        mat_operator& L,
        double alpha,
        double beta,
        double tau,
        double eps
); 


#endif // AMGIF_H
