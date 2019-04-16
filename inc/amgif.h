#ifndef AMGIF_H
#define AMGIF_H


#include <memory>
#include <utility>
#include <vector>

#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>

#include "constants.h"
#include "operators_buffer.h"

using std::pair;
using std::shared_ptr;
using std::tuple;
using std::vector;

// Smart pointer to matrix

// Shorthand for template types
using VecTriplet = tuple<gsl_vector *, gsl_vector *, gsl_vector *>;
using OpBuf = OperatorBuffer;

// Compute the AM-GIF estimates
VecTriplet computeAMGIF(
        OpBuf& Cbuf,
        gsl_vector *me,
        gsl_vector *pe,
        const gsl_matrix *L,
        const double alpha,
        const double beta,
        const double tau,
        const double eps
); 


#endif // AMGIF_H
