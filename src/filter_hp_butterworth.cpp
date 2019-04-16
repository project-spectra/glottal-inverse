#include "iaif.h"

#include <memory>
#include <cmath>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

// https://www.dsprelated.com/showarticle/1135.php


using std::pair;
using std::shared_ptr;


static void poly(gsl_complex p[], const gsl_complex z[], const size_t N);


pair<gsl_vector *, gsl_vector *> create_hp_butterworth(const size_t N, const double fc) {
    constexpr double fs(SAMPLE_RATE);

    const double Fc = fs / M_PI * tan(M_PI * fc / fs);

    size_t k, m;

    gsl_complex p[N];
    gsl_complex q[N];

    // I. Find poles of normalized analog lowpass filter
    for (k = 0; k < N; ++k) {
        const double theta = (k * M_PI) / static_cast<double>(N);
       
        GSL_SET_COMPLEX(&p[k], -sin(theta), cos(theta));
    }

    // II. Transform poles for highpass filter
    for (k = 0; k < N; ++k) {
        p[k] = gsl_complex_mul_real(gsl_complex_inverse(p[k]), 2 * M_PI * Fc);
    }
    
    // III. Find coeffs of digital filter, poles and zeros in the Z-plane
    for (k = 0; k < N; ++k) {
        const auto num = gsl_complex_div_real(gsl_complex_add_real(p[k], 1.), 2. * fs);
        const auto den = gsl_complex_div_real(gsl_complex_sub_real(p[k], 1.), 2. * fs);
        p[k] = gsl_complex_div(num, den); 

        GSL_SET_COMPLEX(&q[k], 1., 0.);
    }

    // find polynomial with roots p and q
    gsl_complex a[N+1];
    gsl_complex b[N+1];

    poly(a, p, N);
    poly(b, q, N);

    // amplitude scale factor for gain = 1
    gsl_complex Kscale;
    gsl_complex Knum;
    gsl_complex Kden;

    GSL_SET_COMPLEX(&Knum, 0., 0.);
    GSL_SET_COMPLEX(&Kden, 0., 0.);

    for (m = 0; m <= N; ++m) {
        const double m1m = (m % 2) ? 1. : -1.;

        for (k = 0; k <= N; ++k) {
            Knum = gsl_complex_add(Knum, gsl_complex_mul_real(a[k], m1m));
            Knum = gsl_complex_add(Knum, gsl_complex_mul_real(b[k], m1m));
        }
    }

    Kscale = gsl_complex_div(Knum, Kden);

    const double K = gsl_complex_abs(Kscale);

    static shared_ptr<gsl_vector> A;
    static shared_ptr<gsl_vector> B;

    A.reset(gsl_vector_alloc(N + 1), gsl_vector_free);
    B.reset(gsl_vector_alloc(N + 1), gsl_vector_free);

    for (k = 0; k <= N; ++k) {
        gsl_vector_set(A.get(), k, K * GSL_REAL(a[k]));
        gsl_vector_set(B.get(), k, GSL_REAL(b[k]));
    }

    return { B.get(), A.get() };
}


static void poly(gsl_complex p[], const gsl_complex z[], const size_t N)
{
    size_t j, k;

    for (k = 0; k < N; ++k) {
        GSL_SET_COMPLEX(&p[k], (k == 0 ? 1. : .0), 0.);
    }

    // find characteristic polynomial 
    for (j = 0; j < N; ++j)
    {
        for (k = 0; k <= j; ++k)
        {
            p[k+1] = gsl_complex_sub(p[k+1], gsl_complex_mul(z[j], p[k]));
        }
    }

}
