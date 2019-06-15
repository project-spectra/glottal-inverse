#include "lpc.h"
#include "filter_design.h"
#include "filter.h"
#include "window.h"
#include "audio.h"
#include "iaif.h"


// LPC analysis order for vocal tract
//static size_t p_vt;

// LPC analysis order for glottal source
//static size_t p_gl;

// Leaky integration coefficient
static constexpr double d = 0.98;

// How many times to apply highpass filter
static constexpr size_t hpfilt = 1;

static constexpr double fc = 50.;  // Cutoff frequency for the high-pass filter
static constexpr size_t Nord = 6; // Butterworth order for the HPF 


static void applyInt(const valarray& x, valarray& res) {
    // The coefficients for the IIR integration filter
    static valarray b = { 1 };
    static valarray a = { 1, -d };
    
    filter_iir(b, a, x, res);
}

static void applyHpf(valarray& x) {
    // The coefficients for the Butterworth HPF filter
    static auto coeffs = filter_butter(Nord, fc, HIGHPASS);

    valarray y;

    filter_iir(coeffs[0], coeffs[1], x, y);

    x = y;
}


void computeIAIF(const valarray& s, valarray& g, valarray& dg) {
    static const int p_vt = (2 * SAMPLE_RATE) / 2000 + 4;
    static const int p_gl = (2 * SAMPLE_RATE) / 4000;

    const size_t M(s.size());
    const size_t preflt = p_vt + 1;

    if (M <= p_vt) {
        return;
    }

    valarray x(s);
    valarray win(hanning(M));

    for (size_t k = 0; k < hpfilt; ++k) {
        applyHpf(x);
    }

    // add the ramp
    valarray signal(preflt + M);
    for (size_t k = 0; k < preflt; ++k) {
        signal[k] = (2. * x[0] * k) / (double) preflt - x[0];
    }
    signal[std::slice(preflt, M, 1)] = x;

    valarray Hg1, Hg2, Hvt1, Hvt2;

    valarray y1(M), y2(M);
    valarray g1(M), g1int(M);

    lpcCoeffs(x * win, 1, Hg1);
    filter_fir(Hg1, signal, y1);
    y1 = y1[std::slice(preflt, M, 1)];

    lpcCoeffs(y1 * win, p_vt, Hvt1);
    filter_fir(Hvt1, signal, g1);
    g1 = g1[std::slice(preflt, M, 1)];
    applyInt(g1, g1int);

    lpcCoeffs(g1int * win, p_gl, Hg2);
    filter_fir(Hg2, signal, y2);
    y2 = y2[std::slice(preflt, M, 1)];

    lpcCoeffs(y2 * win, p_vt, Hvt2);
    filter_fir(Hvt2, signal, dg);
    dg = dg[std::slice(preflt, M, 1)];
    applyInt(dg, g);
}
