#include "gnuplot.h"
#include "audio.h"
#include "lpc.h"
#include "gci_pd.h"

using namespace arma;

void gci_pd(const valarray& signal, std::vector<int>& GCIs) {

    static constexpr double r(1.06);

    static constexpr double threshGCI(.8);
    //static constexpr double threshGOI(.8);
    
    int N(signal.size());

    dvec x = zeros<dvec>(N);
    std::copy(std::begin(signal), std::end(signal), x.begin());

    // Pre-process the signal.

    reduceVTI(x);
    //teager(x);
    //reduceDynRange(x);
    removeDC(x);

    // Symmetrize the preprocessed signal, and consider it to be the FT of an even signal.
    cx_vec y = symSignal(x);
    
    // Obtain the inverse FT of the assumed FT, to obtain the even signal.
    y = ifft(y);

    // Consider the causal portion of the even signal and compute the FT, at a radius greater than 1.
    y = fft(y % exp(log(r) * -linspace(0, 2*N-1, 2*N)));

    // Compute the phase-difference spectrum.
    dvec pd = phaseDifference(y);
    pd /= max(pd);

    valarray pdv(pd.memptr(), pd.n_elem);
    writePlotData(pdv, "pd.dat");

    // Threshold and remove GCIs to locate GOIs next.
    GCIs.resize(0);
    selectPeaks(pd, GCIs, threshGCI);

    /*
    // To locate GOIs, repeat the same process on the stripped phase diff spectrum.
    removeDC(pd);
    y = symSignal(pd);
    y = ifft(y);
    y = fft(y % exp(log(r) * -linspace(0, 2*N-1, 2*N)));
    
    dvec pd2 = normalise(phaseDifference(y));
    pd2 /= max(pd2);

    valarray pdv2(pd2.memptr(), pd2.n_elem);
    writePlotData(pdv2, "pd2.dat");

    // Threshold and remove GCIs to locate GOIs next.
    GOIs.resize(0);
    selectPeaks(pd2, GOIs, threshGOI);/
    */

}
