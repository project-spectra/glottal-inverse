#include "iaif.h"


// LPC analysis order for vocal tract
static constexpr size_t p_vt = (2 * SAMPLE_RATE) / 2000 + 4;

// LPC analysis order for glottal source
static constexpr size_t p_gl = (2 * SAMPLE_RATE) / 4000;

// Leaky integration coefficient
static constexpr double d = 0.99;

// How many times to apply highpass filter
static constexpr size_t hpfilt = 1;

// The coefficients for the IIR integration filter
static constexpr double intCoefs[][2] = { { 1 }, { 1, -d } };

static inline void applyWindow(gsl_vector *x, gsl_vector *win) {
    gsl_vector_mul(x, win);
}

static inline void applyInt(gsl_vector *x) {
    static auto b = gsl_vector_const_view_array(intCoefs[0], 1);
    static auto a = gsl_vector_const_view_array(intCoefs[1], 2);
    
    filter_iir(&b.vector, &a.vector, x);
}


std::pair<gsl_vector *, gsl_vector *> computeIAIF(gsl_vector *x) {
    const size_t M = x->size;
//    const size_t preflt = p_vt + 1;

    if (M <= p_vt) {
        return std::pair<gsl_vector *, gsl_vector *>
            (nullptr, nullptr);
    }

    auto dg = gsl_vector_alloc(M);
    auto g = gsl_vector_alloc(M);

    auto signal = gsl_vector_alloc(M);
    auto inter = gsl_vector_alloc(M);
    gsl_vector_memcpy(signal, x);
    gsl_vector_memcpy(inter, x);

    // TODO: apply hpfilter

    auto win = hanning(M);
    
    // TODO: append preflt 

    applyWindow(inter, win);            // inter = xw
    auto Hg1 = lpcCoeffs(inter, 1);     // LPC o/ Hg1
    filter_fir(Hg1, signal, inter);     // inter = y

    applyWindow(inter, win);            // inter = yw
    auto Hvt1 = lpcCoeffs(inter, p_vt); // LPC o/ Hvt1
    filter_fir(Hvt1, signal, inter);    // inter = g1
    applyInt(inter);                    // inter = g1int

    applyWindow(inter, win);            // inter = g1w
    auto Hg2 = lpcCoeffs(inter, p_gl);  // LPC o/ Hg2
    filter_fir(Hg2, signal, inter);     // inter = y2
    applyInt(inter);                    // inter = y2int

    applyWindow(inter, win);            // inter = y2w
    auto Hvt2 = lpcCoeffs(inter, p_vt); // LPC o/ Hvt2

    // calculate dg
    filter_fir(Hvt2, signal, dg);

    // calculate g
    gsl_vector_memcpy(g, dg);
    applyInt(g);

    // free resources
    gsl_vector_free(signal);
    gsl_vector_free(inter);
    gsl_vector_free(win);
    gsl_vector_free(Hg1);
    gsl_vector_free(Hvt1);
    gsl_vector_free(Hg2);
    gsl_vector_free(Hvt2);

    return std::pair<gsl_vector *, gsl_vector *>(dg, g);
}
