#include "gci_yaga_subroutines.h"


void cand_find_psp(const valarray& gamma, candvec& cands)
{
    const size_t N(gamma.size());
    size_t k;

    valarray df1(gamma.shift(1) - gamma);
    valarray df2(df1.shift(1) - df1);

    valarray dfSign(df1 * df1.shift(1));

    // indexes
    valarray idx(N);
    for (k = 0; k < N; ++k)
        idx[k] = k;

    // find turning points
    std::valarray<bool> turning(dfSign < 0.);

    valarray turn_idx(idx[turning]);
    valarray turn_df2(df2[turning]);
    valarray turn_gamma(gamma[turning]);
    
    // find maxima which are < 0
    std::valarray<bool> negMaxima(turn_df2 < 0. && turn_gamma < 0.);
    
    // midpoint between preceding min and negative max
    valarray negIdx(idx[negMaxima]);
    valarray negMid(negIdx.shift(-1) + .5 * (negIdx - negIdx.shift(-1)));
    for (double& x : negMid) {
        size_t index = static_cast<size_t>(x);
        double value = turn_gamma[index];
        
        cands.emplace_back(index - round(value), false);
    }

    // find minima which are > 0
    std::valarray<bool> posMinima(turn_df2 > 0. && turn_gamma > 0.);

    // midpoint between positive min and following max
    valarray posIdx(idx[posMinima]);
    valarray posMid(posIdx + .5 * (posIdx.shift(1) - posIdx));
    for (double& x : posMid) {
        size_t index = static_cast<size_t>(x);
        double value = turn_gamma[index];
        
        cands.emplace_back(index - round(value), false);
    }
}

