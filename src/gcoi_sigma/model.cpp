#include <cmath>
#include "audio.h"
#include "gcoi_sigma_subs.h"


template<int dir>
void gcoi_sigma_model(const arma::dvec& gic, const valarray& crmp, const valarray& grdel, arma::dmat& fv)
{
    const int R = (SIGMA_gwlen * SAMPLE_RATE) / 2 - 1;  

    valarray mgrdel(2 * R + 1);
    for (int i = 0; i < 2 * R + 1; ++i) {
        mgrdel[i] = i - R;
    }

    fv.resize(gic.n_elem, 3);

    for (int i = 0; i < gic.size(); ++i) 
    {
        int lo = std::round(gic[i] - R);
        int hi = std::min<int>(lo + 2 * R, crmp.size() - 1);

        if (lo >= 0 && hi <= grdel.size()) {
            valarray crmpSeg = crmp[std::slice(lo, hi - lo + 1, 1)];
            valarray grdelSeg = grdel[std::slice(lo, hi - lo + 1, 1)];
            
            fv(i, 0) = crmpSeg.sum();                           // Sum of crmp over GD window
            fv(i, 1) = dir < 0 ? crmpSeg.min() : crmpSeg.max(); // Peak value of crmp
            
            valarray meanSeg(mgrdel - grdelSeg);
            
            fv(i, 2) = sqrt( (meanSeg * meanSeg).sum() / (double) meanSeg.size() ); // Phase slope deviation
        }
    }
}

template void gcoi_sigma_model<-1>(const arma::dvec&, const valarray&, const valarray&, arma::dmat&);
template void gcoi_sigma_model<1>(const arma::dvec&, const valarray&, const valarray&, arma::dmat&);
