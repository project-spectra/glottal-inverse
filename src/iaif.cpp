#include "constants.h"
#include "lpc.h"
#include "filter.h"
#include "window.h"
#include "iaif.h"


// LPC analysis order for vocal tract
static constexpr size_t p_vt = (2 * SAMPLE_RATE) / 2000 + 4;

// LPC analysis order for glottal source
static constexpr size_t p_gl = (2 * SAMPLE_RATE) / 4000;

// Leaky integration coefficient
static constexpr double d = 0.99;

// How many times to apply highpass filter
static constexpr size_t hpfilt = 1;

// Highpass cutoff frequency
static constexpr double fc = 70.;

static void applyInt(const valarray& x, valarray& res) {
    // The coefficients for the IIR integration filter
    static valarray b = { 1 };
    static valarray a = { 1, -d };
    
    filter_iir(b, a, x, res);
}


void computeIAIF(const valarray& signal, valarray& g, valarray& dg) {
    const size_t M(signal.size());
    // const size_t preflt = p_vt + 1;  // unused

    if (M <= p_vt) {
        return;
    }

    valarray x(signal);
    valarray win(hanning(M));

    for (size_t it = 0; it < hpfilt; ++it) {
        filter_hpf(x, fc);
    }

    valarray Hg1, Hg2, Hvt1, Hvt2;

    valarray y1(M), y2(M), y2int(M);
    valarray g1(M), g1int(M);

    lpcCoeffs(x * win, 1, Hg1);
    filter_fir(Hg1, x, y1);

    lpcCoeffs(y1 * win, p_vt, Hvt1);
    filter_fir(Hvt1, x, g1);
    applyInt(g1, g1int);

    lpcCoeffs(g1int * win, p_gl, Hg2);
    filter_fir(Hg2, x, y2);
    applyInt(y2, y2int);

    lpcCoeffs(y2int * win, p_vt, Hvt2);
    // calculate dg
    filter_fir(Hvt2, x, dg);
    // calculate g
    applyInt(dg, g);
}
