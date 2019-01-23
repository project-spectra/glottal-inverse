#include "iaif.h"


// LPC analysis order for vocal tract
constexpr size_t p_vt = (2 * SAMPLE_RATE) / 2000 + 4;

// LPC analysis order for glottal source
constexpr size_t p_gl = (2 * SAMPLE_RATE) / 4000;

// Leaky integration coefficient
constexpr double d = 0.99;

// How many times to apply highpass filter
constexpr size_t hpfilt = 1;

// The coefficients for the IIR integration filter
constexpr double intCoefs[][2] = { { 1 }, { 1, -d } };

static gsl_vector *applyWindow(gsl_vector *x, gsl_vector *win) {
    gsl_vector *xw;
    xw = gsl_vector_alloc(x->size);
    gsl_vector_memcpy(xw, x);
    gsl_vector_mul(xw, win);
    return xw;
}

static inline gsl_vector *applyInt(gsl_vector *x, size_t n) {
    static auto b = gsl_vector_const_view_array(intCoefs[0], 1);
    static auto a = gsl_vector_const_view_array(intCoefs[1], 2);
    
    return filter_iir(&b.vector, 1, &a.vector, 2, x, n);
}


gsl_vector *computeIAIF(gsl_vector *x, size_t M) {
    size_t preflt;

    preflt = p_vt + 1;

    // TODO: apply hpfilt

    if (M > p_vt) {
        auto win = hanning(M);
        auto signal = x;
        
        // TODO: append preflt 

        auto xw = applyWindow(x, win);
        auto Hg1 = lpcCoeffs(xw, M, 1);
        auto y = filter_fir(Hg1, 2, signal, M);
        
        auto yw = applyWindow(y, win);
        auto Hvt1 = lpcCoeffs(yw, M, p_vt);
        auto g1 = filter_fir(Hvt1, p_vt+1, signal, M);
        auto g1int = applyInt(g1, M);

        auto g1w = applyWindow(g1int, win);
        auto Hg2 = lpcCoeffs(g1w, M, p_gl);
        auto y2 = filter_fir(Hg2, p_gl+1, signal, M);
        auto y2int = applyInt(y2, M);

        auto y2w = applyWindow(y2int, win);
        auto Hvt2 = lpcCoeffs(y2w, M, p_vt);
        auto dg = filter_fir(Hvt2, p_vt+1, signal, M);
        
        return dg;
    }

    return nullptr;
}