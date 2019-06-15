#include <climits>
#include <cmath>
#include "audio.h"
#include "filter.h"
#include "window.h"
#include "lpc.h"
#include "normalize.h"
#include "gci_sedreams.h"

using std::vector;


static constexpr int hpfilt = 3;


void gci_sedreams(const valarray& signal, const double T0mean, vector<int>& gci, vector<int>& goi) {

    const int N(signal.size());
    int n, k;

    valarray res;

    lpcResidual(
            signal,
            round(25. / 1000. * SAMPLE_RATE),
            round(5. / 1000. * SAMPLE_RATE),
            round(SAMPLE_RATE / 1000.) + 2,
            res, nullptr
    );

    // filter out any NaNs
    for (n = 0; n < N; ++n) {
        if (!std::isnormal(res[N])) {
            res[n] = 0.;
        }
    }

    // Calculation of the mean-base signal
    int halfL = round((1.75 * T0mean * SAMPLE_RATE) / 2.);
    valarray blackwin(blackman(2 * halfL + 1));
    
    // Filter wave with blackwin and take mean
    valarray meanBasedSignal;

    filter_iir(blackwin, { static_cast<double>(blackwin.size()) }, signal, meanBasedSignal);

    // Remove low frequency contents  TODO:ellipsis IIR fiter
    for (n = 0; n < hpfilt; ++n) {
        filter_hpf(meanBasedSignal, 50. / (SAMPLE_RATE / 2.));
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
   
    vector<int> pzcr, nzcr;
    findZeroCrossings(meanBasedSignal, pzcr, nzcr);

    vector<std::pair<int, int>> gciInter, goiInter;

    // Make the first interval estimates
    int posi(0), negi(0);

    for (int i = 0; i < maxInd.size() - 1; ++i) {
        
        // Find the first positive zero-crossing following the current minimum.
        while (minInd[i] <= pzcr[posi++]);
        // It has to preceed the next minimum.
        if (pzcr[posi] < minInd[i + 1]) {
            gciInter.emplace_back(minInd[i], pzcr[posi]);
        }
        else {
            gciInter.emplace_back(0, -1);
            posi--;
        }

        // Find the first negative zero-crossing following the current maximum.
        while (maxInd[i] <= nzcr[negi++]);
        // It has to preceed the next maximum.
        if (nzcr[negi] < maxInd[i + 1]) {
            goiInter.emplace_back(maxInd[i], nzcr[negi]);
        }
        else {
            goiInter.emplace_back(0, -1);
            negi--;
        }
        
    }

    normalize(res);
  
    constexpr double gciThreshold(0.4);
    constexpr double goiThreshold(0.2);

    // Refine GCIs
    for (auto [start, end] : gciInter) {
       
        if (start > end) {
            gci.push_back(-1);
            continue;
        }

        // Pad the interval with 0.15ms.
        start = std::max((int) (start - (0.15 / 1000. * SAMPLE_RATE)), 0);
        end = std::min((int) (end + (0.15 / 1000. * SAMPLE_RATE)), (int) res.size() - 1);
        
        // Find the local maximum in the LP residual
            
        int maxi(start);
        double maxr(-HUGE_VAL);
        for (int i = start; i <= end; ++i) {
            if (res[i] > gciThreshold && res[i] > maxr) {
                maxr = res[i];
                maxi = i;
            }
        }

        gci.push_back(maxi);
    }
    
    // Refine GOIs
    for (auto [start, end] : goiInter) {
        
        if (start > end) {
            goi.push_back(-1);
            continue;
        }

        // Pad the interval with 0.15ms.
        start = std::max((int) (start - (0.15 / 1000. * SAMPLE_RATE)), 0);
        end = std::min((int) (end + (0.15 / 1000. * SAMPLE_RATE)), (int) res.size() - 1);

        // Find the local maximum in the LP residual
            
        int maxi(start);
        double maxr(-HUGE_VAL);
        for (int i = start; i <= end; ++i) {
            if (res[i] > goiThreshold && res[i] > maxr) {
                maxr = res[i];
                maxi = i;
            }
        }

        goi.push_back(maxi);
    }

}
