#include <cmath>
#include "window.h"

valarray hanning(const size_t L)
{
    valarray win(L);

    const size_t N(L - 1);

    size_t n;
    for (n = 0; n <= N; ++n) {
        win[n] = n;
    }

    win = .5 * (1. - cos(2. * M_PI * n / (double) N));

    return win;
}
