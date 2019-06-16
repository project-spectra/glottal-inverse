#include "audio.h"
#include "filter.h"
#include "lpc.h"
#include "window.h"
#include "dyprog_lf.h"


arma::dvec get_spec_stat(const valarray& x, std::vector<int>& GCIs) {
    // Spectral stationarity measure.
    
    static constexpr int med_len = 7;
    static constexpr int sm_len = 7;

    static const int lpc_ord = 2 + round(SAMPLE_RATE / 1000.);

    static const int frameLen = round(25. / 1000. * SAMPLE_RATE);
    static const int frameShift = round(10. / 1000. * SAMPLE_RATE);

    valarray win = hamming(frameLen);

    valarray sp;
    filter_fir({ 1., -0.99 }, x, sp);

    arma::dvec ss = arma::zeros<arma::dvec>(round((x.size() - frameLen) / frameShift));

    int start = 0;
    int stop = start + frameLen - 1;

    valarray segPrev(sp[std::slice(start, stop, 1)]);
    segPrev *= win;

    valarray lpcPrev;
    lpcCoeffs(segPrev, lpc_ord, lpcPrev);

    int cnt = 0;
    while (stop < x.size()) {
        start = start + frameShift - 1;
        stop = start + frameLen - 1;

        if (stop >= sp.size()) {
            break;
        }
        
        valarray segCur(sp[std::slice(start, stop, 1)]);
        segCur *= win;

        valarray lpcCur;
        lpcCoeffs(segCur, lpc_ord, lpcCur);

        // Calculate spectral stationarity
        ss(cnt) = 0.2 / (distitar(lpcCur, lpcPrev) - 0.8);
    }

	return ss;
}
