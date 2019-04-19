#include "gci_dypsa.h"


vector<pair<size_t, double>>
    negativeZeroCrossings(gsl_vector *y)
{
    const size_t N(y->size);

    vector<pair<size_t, double>> crossings;

    size_t n;
    double prev, cur;

    prev = gsl_vector_get(y, 0);
    for (n = 1; n < N; ++n) {
        cur = gsl_vector_get(y, n);

        if (prev >= 0 && cur < 0) {
            crossings.emplace_back(n, cur - prev);
        }

        prev = cur;
    }

    return crossings;
}
