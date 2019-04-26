#include <cmath>
#include "window.h"

valarray hamming(const size_t L)
{
    valarray win(L);

    const size_t N(L - 1);

    size_t n;
    for (n = 0; n <= N; ++n) {
        win[n] = n / (double) N;
    }

    win = .54 - .46 * cos(2. * M_PI * win);

    return win;
}
