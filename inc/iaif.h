#ifndef IAIF_H
#define IAIF_H


#include <valarray>

using valarray = std::valarray<double>;

void computeIAIF(const valarray& x, valarray& g, valarray& dg);


#endif // IAIF_H
