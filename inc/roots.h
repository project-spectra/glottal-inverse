#ifndef ROOTS_H
#define ROOTS_H


#include <complex>
#include <valarray>

using valarray = std::valarray<double>;

using complex = std::complex<double>;
using cvalarray = std::valarray<complex>;


cvalarray roots(const valarray& p);


#endif // ROOTS_H
