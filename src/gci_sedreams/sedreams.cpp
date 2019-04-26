#include <climits>
#include <cmath>
#include "filter.h"
#include "window.h"
#include "lpc.h"
#include "normalize.h"
#include "gci_sedreams.h"

using std::vector;


static constexpr int hpfilt = 3;


vector<int> gci_sedreams(const valarray& signal, const double fs, const double f0mean) {

    const int N(signal.size());
    int n, k;

    valarray res;

    lpcMaxResidual(signal, round(fs/1000.) + 2, res);

    // filter out any NaNs
    for (n = 0; n < N; ++n) {
        if (!std::isfinite(res[N])) {
            res[n] = 0.;
        }
    }

    // Calculation of the mean-base signal
    int T0mean = round(fs / f0mean);
    int halfL = round((1.7 * T0mean) / 2.);
    valarray blackwin(blackman(2 * halfL + 1));
    
    // Filter wave with blackwin and take mean
    valarray meanBasedSignal;

    filter_iir(blackwin, { static_cast<double>(blackwin.size()) }, signal, meanBasedSignal);

    // Remove low frequency contents  TODO:ellipsis IIR fiter
    for (n = 0; n < hpfilt; ++n) {
        filter_hpf(meanBasedSignal, 50./(fs/2));
    }
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
    vector<int> posInd;
    constexpr double resThreshold(0.4);
    for (n = 0; n < N; ++n) {
        if (res[n] > resThreshold) {
            posInd.push_back(n);
        }
    }
    posInd.shrink_to_fit();

    // relative positive indices
    const int posLen(posInd.size());
    vector<double> relPosInd(posLen, 0);

    for (n = 0; n < posLen; ++n) {
        // pos = min_k { abs(minInd[k] - posInd[n]) }
    
        int pos(-1);
        int val, minVal(INT_MAX);

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
    const int minLen(minInd.size());
    vector<int> gci(minLen, 0);

    int maxVal, minVal;
    int interv;
    double alpha;

    int ind(0);
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
            int maxi(start);
            double maxr(-HUGE_VAL);
            for (int i = start; i <= stop; ++i) {
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
