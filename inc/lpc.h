#ifndef LPC_H
#define LPC_H


#include <valarray>

using valarray = std::valarray<double>;
using valmatrix = std::valarray<valarray>;


double lpcCoeffs(const valarray& x, size_t order, valarray& lpc);

void lpcResidual(const valarray& x, size_t L, size_t shift, size_t order, valarray& res, valmatrix *lpc);


#endif // LPC_H
