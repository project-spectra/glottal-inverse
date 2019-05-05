#ifndef GCOI_SIGMA_H
#define GCOI_SIGMA_H

#include <valarray>
#include <vector>

using valarray = std::valarray<double>;

void gcoi_sigma(const valarray& signal, std::vector<int>& gci, std::vector<int>& goi);


#endif // GCOI_SIGMA_H
