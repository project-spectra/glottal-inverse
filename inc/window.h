#ifndef WINDOW_H
#define WINDOW_H


#include <valarray>

using valarray = std::valarray<double>;

valarray hanning(const size_t L);

valarray hamming(const size_t L);

valarray blackman(const size_t L);


#endif // WINDOW_H
