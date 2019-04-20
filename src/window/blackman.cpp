#include <cmath>
#include <gsl/gsl_math.h>
#include "window.h"

gsl_vector *blackman(const size_t L)
{
    auto win = gsl_vector_alloc(L);

    const size_t N(L - 1);

    constexpr double a0(0.42);
    constexpr double a1(0.5);
    constexpr double a2(0.08);

    size_t n;
    double data, ft, st;

    for (n = 0; n <= N; ++n) {
        ft = a1 * cos((2. * M_PI * n) / (double) N);
        st = a2 * cos((4. * M_PI * n) / (double) N);

        data = a0 - ft + st;

        gsl_vector_set(win, n, data);
    }

    return win;
}
