#include <cmath>
#include "audio.h"
#include "pitch.h"


static constexpr double minFrequency(80.);
static constexpr double maxFrequency(500.);
static constexpr double ratio(5.0);
static constexpr double sensitivity(0.1);



DECL_PITCH(AMDF) {
    static const double minPeriod = round(SAMPLE_RATE / maxFrequency + .5);
    static const double maxPeriod = round(SAMPLE_RATE / minFrequency + .5);

    double f0(-1);
    double minVal(HUGE_VAL);  // +inf
    double maxVal(-HUGE_VAL); // -inf

    const size_t maxShift(frame.size());

    valarray amd(maxShift);

    for (size_t i = 0; i < maxShift; ++i) {
        
        valarray frame1(frame[std::slice(0, maxShift - i, 1)]);
        valarray frame2(frame[std::slice(i, maxShift - i, 1)]);

        amd[i] = abs(frame1 - frame2).sum();
    }
    
    valarray amdSub(amd[std::slice(minPeriod, maxPeriod - minPeriod, 1)]);
    minVal = amdSub.min();
    maxVal = amdSub.max();
    
    size_t cutoff = round((sensitivity * (maxVal - minVal)) + minVal);
    size_t j = minPeriod;

    while (j <= (size_t) maxPeriod && amd[j] > cutoff) {
        j++;
    }

    size_t searchLength(minPeriod / 2);
    minVal = amd[j];
    
    size_t minPos(j);
    size_t i(j);

    while ((i < j + searchLength) && (i <= maxPeriod)) {
        i++;

        double cur(amd[i]);

        if (cur < minVal) {
            minVal = cur;
            minPos = i;
        }
    }

    if (round(amd[minPos] * ratio) < maxVal) {
        f0 = SAMPLE_RATE / minPos;
    }

    *f0res = f0;
    *T0res = 1. / f0;

    return f0 != -1;

}
