#ifndef VTL_H
#define VTL_H


#include <complex>
#include <valarray>

using complex = std::complex<double>;
using valarray = std::valarray<double>;
using cvalarray = std::valarray<complex>;


double estimateVTL(const valarray& u);


#endif // VTL_H
