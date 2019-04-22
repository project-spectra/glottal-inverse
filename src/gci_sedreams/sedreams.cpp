#include <cstdint>
#include <cmath>
#include "filter.h"
#include "window.h"
#include "lpc.h"
#include "normalize.h"
#include "gci_sedreams.h"

using std::vector;

vector<size_t> gci_sedreams(const valarray& signal, const double fs, const double f0mean) {

    const size_t N(signal.size());
    size_t n, k;

    valarray res;

    lpcResidual(signal,
                    round(25./1000.*fs),
                    round(5./1000.*fs),
                    round(fs/1000.) + 2,
                    res, nullptr);

    // filter out any NaNs
    for (n = 0; n < N; ++n) {
        if (!std::isfinite(res[N])) {
            res[n] = 0.;
        }
    }

    // Calculation of the mean-base signal
    size_t T0mean = round(fs / f0mean);
    size_t halfL = round((1.7 * T0mean) / 2.);
    valarray blackwin(blackman(2 * halfL + 1));
    
    // Filter wave with blackwin and take mean
    valarray meanBasedSignal;

    filter_iir(blackwin, { static_cast<double>(blackwin.size()) }, signal, meanBasedSignal);

    // Remove low frequency contents  TODO:ellipsis IIR fiter
    filter_hpf(meanBasedSignal, 50./(fs/2.));
    normalize(meanBasedSignal);

    // Detect minima and maxima of the mean-based signal
    auto maxInd = findPeaks(meanBasedSignal, 1.);
    auto minInd = findPeaks(meanBasedSignal, -1.);

    while (maxInd.front() < minInd.front()) {
        maxInd.pop_front();
    }
    while (minInd.back() > maxInd.back()) {
        minInd.pop_back();
    }
    maxInd.shrink_to_fit();
    
    // Determine the median position of GCIs within the cycle
    normalize(res);

    // find points of res > threshold
    vector<size_t> posInd;
    constexpr double resThreshold(0.4);
    for (n = 0; n < N; ++n) {
        if (res[n] > resThreshold) {
            posInd.push_back(n);
        }
    }
    posInd.shrink_to_fit();

    // relative positive indices
    const size_t posLen(posInd.size());
    vector<double> relPosInd(posLen, 0);

    for (n = 0; n < posLen; ++n) {
        // pos = min_k { abs(minInd[k] - posInd[n]) }
    
        size_t pos(-1);
        size_t val, minVal(SIZE_MAX);

        for (k = 0; k < minInd.size(); ++k) {
            val = abs((int) minInd[k] - (int) posInd[n]);
            if (val < minVal) {
                pos = k;
                minVal = val;
            }
        }

        double num = (double) posInd[n] - (double) minInd[pos];
        double den = (double) maxInd[pos] - (double) minInd[pos];

        relPosInd[n] = num / den;
    }
    
    double ratioGCI = median(relPosInd);

    // Detect GCIs *and* GOIs from the residual signal using the presence intervals derived from the mean signal
    const size_t minLen(minInd.size());
    vector<size_t> gci(minLen, 0);

    int maxVal, minVal;
    int interv;
    double alpha;

    size_t ind(0);
    int start, stop;

    for (n = 0; n < minLen; ++n) {
        maxVal = maxInd[n];
        minVal = minInd[n];
        interv = maxVal - minVal;

        alpha = ratioGCI - 0.35;
        start = minVal + round(alpha * interv);

        alpha = ratioGCI + 0.35;
        stop = minVal + round(alpha * interv);
      
        if (start < 1) {
            start = 1;
        } else if (start > N) {
            break;
        }
        if (stop > N) {
            stop = N;
        }

        if (stop > 1) {
            size_t maxi(start);
            double maxr(-HUGE_VAL);
            for (size_t i = start; i <= stop; ++i) {
                if (res[i] > maxr) {
                    maxr = res[i];
                    maxi = i;
                }
            }

            gci[ind++] = maxi;
        }
    }

    return gci;
}
