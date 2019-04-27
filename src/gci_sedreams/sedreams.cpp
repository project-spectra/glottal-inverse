#include <climits>
#include <cmath>
#include <tuple>
#include "audio.h"
#include "filter.h"
#include "window.h"
#include "lpc.h"
#include "normalize.h"
#include "gci_sedreams.h"

using std::vector;


static constexpr int hpfilt = 3;


void gci_sedreams(const valarray& signal, const double T0mean, vector<int>& gci, vector<int>& goi) {

    // 0.15ms margin on the GCI interval
    const int gciMargin = 0.15 / 1000. * SAMPLE_RATE;
    
    // 0.25ms margin on the GOI interval
    const int goiMargin = 0.25 / 1000. * SAMPLE_RATE;
    
    const int N(signal.size());

    valarray res;

    lpcResidual(
            signal,
            round(25. / 1000. * SAMPLE_RATE),
            round(5. / 1000. * SAMPLE_RATE),
            round(SAMPLE_RATE / 1000.) + 2,
            res, nullptr
    );

    // filter out any NaNs
    for (int n = 0; n < N; ++n) {
        if (!std::isfinite(res[N])) {
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
    for (int n = 0; n < hpfilt; ++n) {
        filter_hpf(meanBasedSignal, 50. / (SAMPLE_RATE / 2.));
    }
    normalize(meanBasedSignal);

    // Detect minima and maxima of the mean-based signal
    auto maxima = findPeaks(meanBasedSignal, 1.);
    auto minima = findPeaks(meanBasedSignal, -1.);
    
    while (maxima.front() < minima.front()) {
        maxima.pop_front();
    }
    while (minima.back() > maxima.back()) {
        minima.pop_back();
    }

    maxima.shrink_to_fit();
    minima.shrink_to_fit();

    std::vector<int> posZcrs, negZcrs;
    findZeroCrossings(meanBasedSignal, posZcrs, negZcrs);

    // Find GCI and GOI intervals
    std::vector<std::pair<int, int>> gciInterv, goiInterv;

    for (int nc = 0; nc < minima.size(); ++nc) {
        std::pair<int, int> gci, goi;

        gci.first = minima[nc];
        // the first following positive zero-crossing
        for (int t : posZcrs) {
            if (t > gci.first) {
                gci.second = t;
                break;
            }
        }
        
        // Pad the GOI interval with a 0.25ms margin
        /*gci.first = std::max(0, gci.first - gciMargin);
        gci.second = std::min(N, gci.second + gciMargin);*/

        goi.first = maxima[nc];
        // the first following negative zero-crossing
        for (int t : negZcrs) {
            if (t > goi.first) {
                goi.second = t;
            }
        }

        // Pad the GOI interval with a 0.25ms margin
        /*goi.first = std::max(0, goi.first - goiMargin);
        goi.second = std::min(N, goi.second + goiMargin);*/

        printf("GOI interv %d :  %d - %d \n", nc, goi.first, goi.second);

        gciInterv.push_back(gci);
        goiInterv.push_back(goi);
    }

    // Detect GCIs and GOIs from the residual signal using the presence intervals derived from the mean signal
    const int nbCand(minima.size());
    gci.resize(0);
    goi.resize(0);

    for (int nc = 0; nc < nbCand; ++nc) {
        int start, stop;
        int maxi;
        double maxr;

        // Get the GCI
        std::tie(start, stop) = gciInterv[nc];

        maxi = start;
        maxr = -HUGE_VAL;
        for (int i = start; i <= stop; ++i) {
            if (res[i] > maxr) {
                maxr = res[i];
                maxi = i;
            }
        }

        gci.push_back(maxi);

        // Get the GOI
        std::tie(start, stop) = goiInterv[nc];

        maxi = start;
        maxr = -HUGE_VAL;
        for (int i = start; i <= stop; ++i) {
            if (res[i] > maxr) {
                maxr = res[i];
                maxi = i;
            }
        }

        goi.push_back(maxi);
    }
}
