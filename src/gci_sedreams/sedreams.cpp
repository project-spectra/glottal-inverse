#include <cstdint>
#include <cmath>
#include "filter.h"
#include "window.h"
#include "lpc.h"
#include "normalize.h"
#include "gci_sedreams.h"

using std::vector;

vector<size_t> gci_sedreams(gsl_vector *signal, const double fs, const double f0mean) {

    const size_t N(signal->size);
    size_t n, k;

    gsl_vector *res = gsl_vector_alloc(N);

    lpcResidual(nullptr, res, signal,
                    round(25./1000.*fs),
                    round(5./1000.*fs),
                    round(fs/1000.) + 2);

    // filter out any NaNs
    for (n = 0; n < N; ++n) {
        if (!std::isfinite(gsl_vector_get(res, n))) {
            gsl_vector_set(res, n, 0.);
        }
    }

    // Calculation of the mean-base signal
    size_t T0mean = round(fs / f0mean);
    size_t halfL = round((1.7 * T0mean) / 2.);
    auto blackwin = blackman(2 * halfL + 1);
    
    // Filter wave with blackwin and take mean
    auto meanBasedSignal = gsl_vector_alloc(N);

    double filtDen[] = { static_cast<double>(blackwin->size) };
    auto filtDenView = gsl_vector_view_array(filtDen, 1);

    filter_iir(blackwin, &filtDenView.vector, signal, meanBasedSignal);
    gsl_vector_free(blackwin);

    // Remove low frequency contents  TODO:ellipsis IIR fiter
    filter_hpf(meanBasedSignal, 50./(fs/2.));
    normalize(meanBasedSignal, 1.);

    // Detect minima and maxima of the mean-based signal
    auto maxInd = findPeaks(meanBasedSignal, 1.);
    auto minInd = findPeaks(meanBasedSignal, -1.);

    gsl_vector_free(meanBasedSignal);

    while (maxInd.front() < minInd.front()) {
        maxInd.pop_front();
    }
    while (minInd.back() > maxInd.back()) {
        minInd.pop_back();
    }
    maxInd.shrink_to_fit();
    
    // Determine the median position of GCIs within the cycle
    normalize(res, 1.);

    // find points of res > threshold
    vector<size_t> posInd;
    constexpr double resThreshold(0.4);
    for (n = 0; n < N; ++n) {
        if (gsl_vector_get(res, n) > resThreshold) {
            posInd.push_back(n);
        }
    }
    posInd.shrink_to_fit();

    // relative positive indices
    const size_t posLen(posInd.size());
    vector<int> relPosInd(posLen, 0);

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

        int num = (int) posInd[n] - (int) minInd[pos];
        int den = (int) maxInd[pos] - (int) minInd[pos];

        relPosInd[n] = num / den;
    }
    
    int ratioGCI = median(relPosInd);

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
            auto vec = gsl_vector_subvector(res, start, stop - start);
            size_t maxi = gsl_vector_max_index(&vec.vector);

            gci[ind++] = (start + maxi - 1);
        }
    }
    
    gsl_vector_free(res);

    return gci;
}
