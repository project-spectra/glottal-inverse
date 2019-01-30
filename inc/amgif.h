#ifndef AMGIF_H
#define AMGIF_H


#include <utility>
#include <vector>
#include <memory>

#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_spmatrix.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_monte_miser.h>

#include "constants.h"

using std::pair;
using std::vector;
using std::unique_ptr;

// Smart pointer to matrix
using mat_operator = unique_ptr<gsl_spmatrix, void(*)(gsl_spmatrix *)>;

// Generate operator L
mat_operator computeL();

// Generate operator C
vector<mat_operator> computeC();
void computeSingleC(gsl_spmatrix *C, size_t mu);

// Compute the AM-GIF estimates
pair<gsl_vector *, gsl_vector *> computeAMGIF(
        vector<mat_operator>& C,
        gsl_vector *me,
        gsl_vector *pe,
        mat_operator& L,
        double alpha,
        double beta,
        double tau,
        double eps
); 


#endif // AMGIF_H
