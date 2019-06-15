#include "audio.h"
#include "filter_design.h"

using namespace arma;

void filter_butter_ap(int N, filter_zpk& dst)
{
    dst.z = {};
    dst.p = exp(cx_double(0., 1.)  * (M_PI * regspace<cx_vec>(1, 2, 2 * N - 1) / (2. * N) + M_PI / 2.));
    dst.k = real(prod(-dst.p));
}

std::array<valarray, 2> filter_butter(int N, double fc, filter_type fType)
{
    // Step 1. Get analog, pre-warped frequencies. Digital filter.
    double Wn = tan(M_PI * fc / SAMPLE_RATE);

    // Step 2. Get N-th order Butterworth analog lowpass prototype.
    filter_zpk buttap;
    filter_butter_ap(N, buttap);

    // Step 3. Transform to state-space.
    filter_ss buttap_ss;
    convert_zpk2ss(buttap, buttap_ss);

    // Step 4. Transform to lowpass or highpass filter of desired Wn.
    filter_ss butta_ss;
    switch (fType) {
        case LOWPASS:
            transform_lp2lp(buttap_ss, Wn, butta_ss);
            break;
        case HIGHPASS:
            transform_lp2hp(buttap_ss, Wn, butta_ss);
            break;
    }

    // Step 5. Use bilinear transformation to find discrete equivalent.
    filter_ss butt_ss;
    convert_bilinear(butta_ss, butt_ss);
   
    // Step 6. Convert to transfer function.
    filter_tf butt_tf;
    convert_ss2tf(butt_ss, butt_tf);

    // Step 7. Convert to real transfer function.
    vec num = real(butt_tf.b);
    vec den = real(butt_tf.a);

    return {
        valarray(num.memptr(), num.n_elem),
        valarray(den.memptr(), den.n_elem)
    };
}
