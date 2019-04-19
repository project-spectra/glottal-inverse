#include "gci_dypsa.h"


tuple<vector<pair<size_t, double>>, gsl_vector *, double>
    extractGroupDelayEpoch(gsl_vector *u, const double fs)
{
    size_t n;
    double data;
   
    // hanning window
    size_t gw = 2 * floor(dy_gwlen * fs / 2.) + 1;
    auto ghw = hanning(gw);

    // weighted window
    auto ghwn = gsl_vector_alloc(ghw->size);
    for (n = 0; n <= gw; ++n) {
        data = gsl_vector_get(ghw, n) * (gw - 1. - 2.*n) / 2.;
        gsl_vector_set(ghwn, n, data);
    }

    // u*u
    auto u2 = gsl_vector_alloc(u->size);
    gsl_vector_memcpy(u2, u);
    gsl_vector_mul(u2, u);

    // yn, yd
    auto yn = gsl_vector_alloc(u2->size);
    auto yd = gsl_vector_alloc(u2->size);
    filter_fir(ghwn, u2, yn);
    filter_fir(ghw, u2, yd);

    // TODO: prevent infinities
    
    // delete filter startup transient
    auto y = gsl_vector_alloc(u2->size - gw);
    {
        auto ynView = gsl_vector_subvector(yn, gw, yn->size - gw);
        auto ydView = gsl_vector_subvector(yd, gw, yd->size - gw);

        gsl_vector_memcpy(y, &ynView.vector);
        gsl_vector_div(y, &ydView.vector);
    }
    double toff = (gw - 1.) / 2.;

    // force window length to be odd
    size_t fw = 2 * floor(dy_fwlen * fs / 2.) + 1;
    if (fw > 1) {
        auto daw = hanning(fw);
        filter_fir(daw, y, y);
       
        // divide by sum(daw)
        data = 0.;
        for (n = 0; n < fw; ++n) {
            data += gsl_vector_get(daw, n);
        }
        gsl_vector_scale(y, 1. / data);
        
        toff -= (fw - 1.) / 2.;

        gsl_vector_free(daw);
    }

    auto zcs = negativeZeroCrossings(y);

    gsl_vector_free(ghw);
    gsl_vector_free(ghwn);
    gsl_vector_free(yn);
    gsl_vector_free(yd);

    return { zcs, y, toff };
}
