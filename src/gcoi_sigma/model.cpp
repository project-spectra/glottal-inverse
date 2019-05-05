#include "audio.h"
#include "gcoi_sigma_subs.h"


template<int dir>
void gcoi_sigma_model(const arma::ivec& gic, const valarray& crmp, const valarray& grdel, arma::dmat& fv)
{
    const int R = SIGMA_gwlen * SAMPLE_RATE;

    valarray mgrdel;
    for (int i = 0; i < 2 * R; ++i) {
        mgrdel[i] = i - (R - 1) / 2;
    }

    fv.resize(3, gic.n_elem);

    for (int i = 0; i < gic.size(); ++i) 
    {
        int lo = round(gic[i] - mgrdelMax);
        int hi = std::min(lo + mgrdelLen, (int) crmp.size()) - 1;

        if (lo >= 0 && hi < grdel.size()) {
            valarray crmpSeg = crmp[std::slice(lo, hi - lo + 1, 1)];
            
            fv(0, i) = crmpSeg.sum();                           // Sum of crmp over GD window
            fv(1, i) = dir < 0 ? crmpSeg.min() : crmpSeg.max(); // Peak value of crmp
            
            valarray seg(mgrdel);
            seg -= grdel[std::slice(lo, hi - lo, 1)];
            
            fv(2, i) = sqrt( (seg * seg).sum() / (double) seg.size() ); // Phase slope deviation
        }
    }
}

template void gcoi_sigma_model<-1>(const arma::ivec&, const valarray&, const valarray&, arma::dmat&);
template void gcoi_sigma_model<1>(const arma::ivec&, const valarray&, const valarray&, arma::dmat&);
