#include <cmath>
#include <vector>
#include "filter.h"
#include "normalize.h"
#include "window.h"
#include "audio.h"
#include "lpc.h"



static double giniMeasure(const valarray& x);


void lpcMaxResidual(const valarray& x, size_t order, valarray& residual)
{
    const size_t L(25. / 1000. * SAMPLE_RATE);
    const size_t shift(10. / 1000. * SAMPLE_RATE);

    size_t start(0);
    size_t stop(start + L); 

    valarray win = hanning(L + 1);
    residual.resize(x.size(), 0.);

    valarray alphas = { -1., -0.7 };
        
    std::vector<valarray> res(alphas.size());
    std::vector<double> val(alphas.size());

    while (stop < x.size())
    {
        valarray seg_orig = x[std::slice(start, L + 1, 1)];

        for (size_t k = 0; k < alphas.size(); ++k) {
            double alpha = alphas[k];
            valarray seg, res1, antiCausal, res2, resFinal, A;

            // The first LP analysis (of order K-2) estimates the LP coefs on the pre-emphasized speech signal
            filter_fir({ 1, alpha }, seg_orig, seg);
            lpcCoeffs(seg * win, order - 2, A);

            // The speech signal is inverse-filtered using the coefs of the first LP analysis
            filter_fir(A, seg_orig * win, res1);
            // The resulting is called ra(n) in the paper

            // First operator of causality inversion
            antiCausal.resize(res1.size());
            std::reverse_copy(std::begin(res1), std::end(res1), std::begin(antiCausal));

            // The second LP analysis (of order 2) and inverse filtering is achieved on ra(-n)
            lpcCoeffs(antiCausal, 2, A);
            filter_fir(A, antiCausal, res2);

            // Final operation of causality inversion
            resFinal.resize(res2.size());
            std::reverse_copy(std::begin(res2), std::end(res2), std::begin(resFinal));

            resFinal *= sqrt((seg_orig * seg_orig).sum() / (resFinal * resFinal).sum());

            val[k] = giniMeasure(resFinal);
            res[k] = resFinal;
        }

        // Choose the value of alpha maximising the Gini index (sparsity measure)
        size_t posi;
        double maxVal(-HUGE_VAL);

        for (size_t k = 0; k < alphas.size(); ++k) {
            if (val[k] > maxVal) {
                posi = k;
                maxVal = val[k];
            }
        }
        
        valarray sparseRes = res[posi];

        if (alphas[posi] < -0.7) {
            // To compensate the polarity inversion due to pre-emphasis
            sparseRes = -sparseRes;
        }

        // Overlap and add
        residual[std::slice(start, L + 1, 1)] += sparseRes;

        start += shift;
        stop += shift;
    }

    normalize(residual);

}


static double giniMeasure(const valarray& u) {
    valarray x = abs(u);
    std::sort(std::begin(x), std::end(x));

    double S = 0.;
    double N(u.size());

    double Ab = x.sum();
    for (double k = 0; k < N; ++k) {
        S += x[k] / Ab * ((N - (k+1) + .5) / N);
    }

    return 1. - 2. * S;
}
