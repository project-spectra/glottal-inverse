#ifndef BASIS_H
#define BASIS_H


#include <cstddef>
#include <cmath>
#include <unordered_map>
#include <memory>
#include <vector>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_spblas.h>
#include <gsl/gsl_spmatrix.h>
#include <gsl/gsl_wavelet.h>

#include "constants.h"

using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;
using std::vector;
using std::unordered_map;
using std::pair;

struct pairhash {
public:
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const
    {
        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
    }
};

using IndPair = pair<size_t, size_t>;
using PtrVector = unique_ptr<double[]>;
using BasisList = vector<PtrVector>;
using ConvMap = unordered_map<IndPair, PtrVector, pairhash>;

void coords(double f[], double u[]);
void uncoords(double u[], double f[]);

void normalize(gsl_vector *f);

double convoluteBasis(size_t p, size_t f, size_t mu);


#endif // WAVELETS_H
