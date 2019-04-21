#include "gci_yaga.h"


void swt_app_det_init(const gsl_vector *u, const size_t J, const size_t j1, std::vector<valarray>& d, std::vector<valarray>& a)
{
    // biorthogonal spline wavelet
    // with one vanishing moment
    // d = g[j] is the detail filter
    // a = h[j] is the approx filter
    
    // Haar wavelet satisfies conditions.

    a.resize(J);
    d.resize(J);

    size_t N, k;
    int j;

    // compute j = J down to j = j1 + 1.
    j = J - 1;
    N = pow(2, j);
    
    double a2k, a2k_1;

    for (k = 0; k < N; ++k) {
        a2k = gsl_vector_get(u, 2*k);
        a2k_1 = gsl_vector_get(u, 2*k+1);

        a[j][k] = (a2k + a2k_1) / M_SQRT2;
        d[j][k] = (a2k - a2k_1) / M_SQRT2;
    }

    // while we're above j1, we only want the a[j]
    while (j >= j1) {
        N /= 2;

        for (k = 0; k < N; ++k) {
            a2k = a[j+1][2*k];
            a2k_1 = a[j+1][2*k+1];

            a[j][k] = (a2k + a2k_1) / M_SQRT2;
        }

        j--;
    }

    // do j1 and then resize a to j1 elts.
    while (j >= 0) {
        N /= 2;
        
        for (k = 0; k < N; ++k) {
            a2k = a[j+1][2*k];
            a2k_1 = a[j+1][2*k+1];

            a[j][k] = (a2k + a2k_1) / M_SQRT2;
            d[j][k] = (a2k - a2k_1) / M_SQRT2;
        }

        j--;
    }

    // shrink to necessary size
    a.resize(j1);
    d.resize(j1);
}
