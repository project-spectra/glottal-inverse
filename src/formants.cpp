#include <iostream>
#include <iterator>

#include "lpc.h"
#include "roots.h"
#include "audio.h"
#include "filter.h"
#include "window.h"

#include "formants.h"


std::vector<formant> estimateFormants(const valarray& u, int nb)
{
    const int lpcOrder = (2. * SAMPLE_RATE) / 2000 + 4;

    valarray x1 = u * hamming(u.size());
    valarray x2;

    filter_iir({ 1 }, { 1, 0.63 }, x1, x2);

    valarray a;
    lpcCoeffs(u, lpcOrder, a);
    
    cvalarray rts = roots(a);

    std::valarray<bool> imagPos(rts.size());
    for (int i = 0; i < rts.size(); ++i) {
        imagPos[i] = (rts[i].imag() >= 0.);
    }
    rts = rts[imagPos];

    std::vector<std::pair<int, double>> freqs(rts.size());
    for (int i = 0; i < rts.size(); ++i) {
        freqs[i] = std::make_pair(i, arg(rts[i]) * (SAMPLE_RATE / (2. * M_PI)));
    }

    std::sort(freqs.begin(), freqs.end(), [](auto& a, auto& b) { return a.second < b.second; });

    std::vector<formant> frms;

    for (int i = 0; i < rts.size(); ++i) {
        int ind = freqs[i].first;
        double freq = freqs[i].second;
        double bw = -.5 * (SAMPLE_RATE / (2. * M_PI)) * log(abs(rts[ind]));
        double mag = abs(rts[ind]);

        if (freq > 90. && bw < 400.)
        {
            frms.push_back({ freq, bw, mag });

            if (--nb < 0)
                break;
        }    
    }

    return frms;
}
