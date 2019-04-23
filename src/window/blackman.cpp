#include <cmath>
#include "window.h"

valarray blackman(const size_t L)
{
    valarray win(L);

    const size_t N(L - 1);

    constexpr double a0(0.42);
    constexpr double a1(0.5);
    constexpr double a2(0.08);

    size_t n;
    for (n = 0; n <= N; ++n) {
        win[n] = n / (double) N;
    }

    win = a0 - a1 * cos(2. * M_PI * win)
             + a2 * cos(4. * M_PI * win);

    return win;
}
