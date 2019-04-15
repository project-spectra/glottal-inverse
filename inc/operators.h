#ifndef OPERATOR_H
#define OPERATOR_H


#include <chrono>
#include <iostream>
#include <utility>
#include <vector>
#include <memory>
#include <string>
#include <sstream>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_spmatrix.h>
#include <gsl/gsl_splinalg.h>

#include "constants.h"

using std::array;
using std::pair;
using std::shared_ptr;
using std::tuple;
using std::vector;
using std::string;

// Smart pointer to matrix
using mat_operator = shared_ptr<gsl_spmatrix>;

// Shorthand for template types
using ComputeStatus = vector<pair<size_t, std::string>>;
using ListCmu = const vector<mat_operator>;
using VecPair = pair<gsl_vector *, gsl_vector *>;
using VecTriplet = tuple<gsl_vector *, gsl_vector *, gsl_vector *>;

// Generate operator L
gsl_matrix *computeL();

// Generate operator C
void computeC();

gsl_spmatrix *computeSingleC(size_t mu);

void findValidCs(vector<bool>& toStore);

void setFilenames(string& raw, string& compressed, size_t mu); 

gsl_spmatrix *loadMat(size_t mu);
void storeMat(const gsl_spmatrix *C_mu, size_t mu);

#endif // OPERATOR_H
