#include <cmath>
#include "filter.h"
#include "normalize.h"
#include "window.h"
#include "lpc.h"


void lpcResidual(const valarray& x, size_t L, size_t shift, size_t order, valarray& res, valmatrix *lpc)
{
    const size_t lenX(x.size());
    const size_t N(L + 1);
    size_t start(0);
    size_t stop(start + L); 
    size_t n;

    res.resize(lenX);
    if (lpc != nullptr) {
        lpc->resize(lenX);
    }

    valarray win = hanning(N);
    
    //  Assumed:
    // dim(lpc) = (order+1, len(x))
    // len(res) = len(x)
    // len(lpc) = len(x)/shift

    n = 1;

    valarray lpcRow(order+1);

    while (stop < lenX)
    {
        valarray xseg = x[std::slice(start, N, 1)];

        lpcCoeffs(xseg * win, order, lpcRow);

        valarray inv;
        filter_fir(lpcRow, xseg, inv);

        if (lpc != nullptr) {
            lpc[n] = lpcRow;
        }

        inv *= sqrt((xseg * xseg).sum() / (inv * inv).sum());

        // overlap and add
        res[std::slice(start, N, 1)] += inv;

        start += shift;
        stop += shift;
        n++;
    }


}
