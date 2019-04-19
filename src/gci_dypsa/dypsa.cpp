#include "gci_dypsa.h"


void gci_dypsa(gsl_vector *signal)
{
    constexpr double fs(SAMPLE_RATE);
    const size_t lenSig(signal->size);

    const size_t lpcOrder(ceil(fs * dy_lpcnf + dy_lpcn));

    const double preEmphCoefs[] = { 1, -exp(-2. * M_PI * dy_preemph / fs) };

    size_t n;

    gsl_vector *s_used = gsl_vector_alloc(lenSig);
    gsl_vector *r = gsl_vector_alloc(lenSig);

    // pre-emphasize signal
    {
        auto view = gsl_vector_const_view_array(preEmphCoefs, 2);
        filter_fir(&view.vector, signal, s_used);
    }

    lpcResidual(nullptr, r, s_used, 25. / (1000. * fs), 5. / (1000. * fs), lpcOrder);

    vector<pair<size_t, double>> zcrCands;
    gsl_vector *gdwav;
    double toff;

    tie(zcrCands, gdwav, toff) = extractGroupDelayEpoch(s_used, fs);

    // offset gdwav by toff
    for (n = gdwav->size; n >= 1; --n) {
        if (n < toff) {
            gsl_vector_set(gdwav, n - 1, 0.);
        } else {
            double prec = gsl_vector_get(gdwav, n - 2);
            gsl_vector_set(gdwav, n - 1, prec);
        }
    }

}
