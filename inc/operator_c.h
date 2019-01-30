#ifndef OPERATOR_C
#define OPERATOR_C


#include <vector>
#include <memory>

#include <gsl/gsl_spmatrix.h>

using std::vector;
using std::unique_ptr;

using mat_operator = unique_ptr<gsl_spmatrix, void(*)(gsl_spmatrix *)>;

void computeC();
void computeSingleC(gsl_spmatrix *C, size_t mu);


#endif // OPERATOR_C
