#include <iostream>

#include "audio.h"
#include "filter.h"
#include "formants.h"
#include "window.h"
#include "vtl.h"


double estimateVTL(const valarray& u)
{
    // Window length of 20ms, shift of 5ms
    static const int winLen = 20. / 1000. * SAMPLE_RATE;
    static const int winShf = 5. / 1000. * SAMPLE_RATE;

    // Min max formants
    static constexpr int minF = 1;
    static constexpr int maxF = 4;

    // Speed of sound @ 37°C @ 1 atm
    static constexpr double c = 35295;

    valarray seg;
    
    int start = 0;
    int winCount = 0;

    double avgL = 0.0;

    auto win(hamming(winLen));

    while (start + winLen < u.size()) {
        auto seg = u[std::slice(start, winLen, 1)];
        auto frms = estimateFormants(seg * win, maxF);

        double L = 0.0;

        for (int i = minF; i <= maxF; ++i) {
            double Lk = (c * (((double) i / 2.) - 0.25)) / frms[i-1].freq;

            L += Lk;
        }

        L /= (double) (maxF - minF + 1);
        
        avgL += L;

        start += winShf;
        winCount++;
    }

    avgL /= (double) winCount;

    return avgL;

}
