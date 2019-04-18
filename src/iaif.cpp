#include "iaif.h"
#include "gsl_util.h"


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

// The coefficients for the IIR integration filter
static constexpr double intCoefs[][2] = { { 1 }, { 1, -d } };


static inline void applyWindow(gsl_vector *x, gsl_vector *win) {
    //gsl_vector_mul(x, win);
}

static void applyInt(gsl_vector *x, gsl_vector *res) {
    static auto b = gsl_vector_const_view_array(intCoefs[0], 1);
    static auto a = gsl_vector_const_view_array(intCoefs[1], 2);
    
    filter_iir(&b.vector, &a.vector, x, res);
}


std::pair<gsl_vector *, gsl_vector *> computeIAIF(gsl_vector *x) {
    const size_t M = x->size;
    // const size_t preflt = p_vt + 1;  // unused

    if (M <= p_vt) {
        return std::pair<gsl_vector *, gsl_vector *>
            (nullptr, nullptr);
    }

    static_vector(signal);
    gsl_vector_memcpy(signal, x);
    
    auto dg = gsl_vector_alloc(M);
    auto g = gsl_vector_alloc(M);

    auto winHan = hanning(M);

    for (size_t it = 0; it < hpfilt; ++it) {
        filter_hpf(signal, fc);
    }

    static_vector2(Hg1, 2);  // 1+1
    static_vector2(Hvt1, p_vt+1);
    static_vector2(Hg2, p_gl+1);
    static_vector2(Hvt2, p_vt+1);

    static_vector(y1);
    static_vector(g1);
    static_vector(g1int);
    static_vector(y2);
    static_vector(y2int);

    applyWindow(signal, winHan);
    lpcCoeffs(Hg1->data, signal, 1);
    filter_fir(Hg1, signal, y1);

    applyWindow(y1, winHan);
    lpcCoeffs(Hvt1->data, y1, p_vt);
    filter_fir(Hvt1, y1, g1);
    applyInt(g1, g1int);

    applyWindow(g1int, winHan);
    lpcCoeffs(Hg2->data, g1, p_gl);
    filter_fir(Hg2, g1, y2);
    applyInt(y2, y2int);

    applyWindow(y2int, winHan);
    lpcCoeffs(Hvt2->data, y2int, p_vt);

    // calculate dg
    filter_fir(Hvt2, signal, dg);
    // calculate g
    applyInt(dg, g);

    // free resources
    gsl_vector_free(winHan);

    return std::pair<gsl_vector *, gsl_vector *>(dg, g);
}
