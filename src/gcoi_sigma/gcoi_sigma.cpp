#include <deque>

#include "gnuplot.h"
#include "print_iter.h"

#include "audio.h"
#include "gcoi_sigma.h"
#include "gcoi_sigma_subs.h"


void gcoi_sigma(const valarray& signal, std::vector<int>& rGci, std::vector<int>& rGoi)
{
    valarray u(signal);

    // Multiscale SWT product
    valarray mp;
    gcoi_sigma_swt(u, mp);
    mp[0] = 0.0;

    // Find third roots
    valarray nmp(mp);
    nmp[nmp > 0.] = 0.; // half-wave rectify on negative half for GCI
    valarray crnmp = gcoi_sigma_cbrt(nmp);

    valarray pmp(mp);
    pmp[pmp < 0.] = 0.; // half-wave rectify on positive half for GOI
    valarray crpmp = gcoi_sigma_cbrt(pmp);

    // Get group delay function
    valarray ngrdel, pgrdel;
    int ntoff, ptoff;

    ntoff = gcoi_sigma_gd(nmp, ngrdel);
    ptoff = gcoi_sigma_gd(pmp, pgrdel);

    ngrdel = -ngrdel.shift(ntoff);
    pgrdel = -pgrdel.shift(ptoff);

    writePlotData(ngrdel, "ngrdel.dat");
    writePlotData(pgrdel, "pgrdel.dat");

    // GCI detection
    arma::dvec gcic;
    gcoi_sigma_nzcr(ngrdel, gcic);
    gcic += ntoff;
   
    std::ofstream("gcic.dat") << gcic << std::endl;

    arma::dmat nfv;
    gcoi_sigma_model<-1>(gcic, crnmp, ngrdel, nfv);

    std::ofstream("nfv.dat") << nfv << std::endl;
    
    arma::ivec ingci;
    int Igci = gcoi_sigma_cluster<-1>(nfv, ingci);

    std::ofstream("ingci.dat") << ingci << std::endl;

    arma::dvec gci = arma::zeros<arma::dvec>(u.size());
    for (int i = 0; i < nfv.n_cols; ++i) {
        if (ingci(i) == Igci) {
            gci(gcic(i)) = 0.5;
        }
    }

    // Post-filter swallows (GCIs only)
    std::deque<int> fgci;
    for (int i = 0; i < gci.n_elem; ++i) {
        if (gci(i) > 0.) {
            fgci.push_back(i);
        }
    }

    if (fgci.size() < 2) {
        return;
    }

    // If a GCI is separated from all others by more than Tmax, delete
    
    // Check the first one
    if (fgci[1] - fgci[0] > SIGMA_Tmax * SAMPLE_RATE) {
        fgci.pop_front();
    }
    // Check the middle
    int i = 1;
    while (i < fgci.size() - 2) {
        if ((fgci[i] - fgci[i - 1] > SIGMA_Tmax * SAMPLE_RATE)
                && (fgci[i + 1] - fgci[i] > SIGMA_Tmax * SAMPLE_RATE)) {
            fgci.erase(fgci.begin() + i);
        }
        i++;
    }
    // Check last one
    if (fgci[fgci.size() - 1] - fgci[fgci.size() - 2] > SIGMA_Tmax * SAMPLE_RATE) {
        fgci.pop_back();
    }

    for (int i = 0; i < gci.n_elem; ++i) {
        if (fgci.front() == i) {
            gci(i) = 0.5;
            fgci.pop_front();
        } else {
            gci(i) = 0.0;
        }
    }

    // GOI detection
    arma::dvec goic;
    gcoi_sigma_nzcr(pgrdel, goic);
    goic += ptoff;
    
    arma::dmat pfv;
    gcoi_sigma_model<1>(goic, crpmp, pgrdel, pfv);
   
    arma::ivec ingoi;
    int Igoi = gcoi_sigma_cluster<1>(pfv, ingoi);
    
    arma::dvec goi = arma::zeros<arma::dvec>(u.size());
    for (int i = 0; i < pfv.n_cols; ++i) {
        if (ingoi(i) == Igoi) {
            goi(goic(i)) = 0.75;
        }
    }

    // GOI post-fitering
    
    // For all GCIs, find GOIs which are within OQ limits
    arma::dvec goifilt = arma::zeros<arma::dvec>(u.size());

    std::vector<int> gciind;
    for (int i = 0; i < gci.n_elem; ++i) {
        if (gci(i) > 0.) {
            gciind.push_back(i);
        }
    }

    int Tprev = SIGMA_Tmax * SAMPLE_RATE;
    int prev = 0;
    bool noFirst = false;

    for (int i = 1; i < gciind.size(); ++i) {
        int lo = gciind[i-1];
        int hi = gciind[i];
        int T = hi - lo;
        
        // If period is too long, use previous
        if (T > SIGMA_Tmax * SAMPLE_RATE) {
            T = Tprev;
        }

        // Find first eventual GOI in the period (with margins for minmax Oq)
        int I = -1;
        for (int k = round(lo + (1. - SIGMA_oqmax) * T); k <= round(lo + (1. - SIGMA_oqmin) * T); ++k) {
            if (goi(k) > 0.) {
                I = k;
                break;
            }
        }

        // If we have a GOI
        if (I != -1) {
            prev = round(I + (1 - SIGMA_oqmax) * T - 1);
            goifilt(round(I + lo + (1 - SIGMA_oqmax) * T - 1)) = 0.5;
        }
        // If not, insert at last OQ
        else {
            if (prev > 0) {
                // Protect against GOI occuring after next GCI
                if (lo + prev < hi) {
                    goifilt(lo + prev - 1) = 0.5;
                } else {
                    goifilt(hi - 1) = 0.5;
                }
            }
            if (i == 1) {
                noFirst = true;
            }
        }

        // If no GOI occurs after GOI, no previous peroid exists, so add after a period has been found.
        if (noFirst && prev > 0) {
            goifilt(gciind[0] + prev - 1) = 0.5;
            noFirst = false;
        }
        
        Tprev = T;
    }

    // Final period
    int lo = gciind[gciind.size()-1];
    int I = -1;
    for (int k = round(lo + (1. - SIGMA_oqmax) * Tprev);
                k < goi.n_elem && k <= round(lo + (1. - SIGMA_oqmin) * Tprev); ++k) {
        if (goi(k) > 0.) {
            I = k;
            break;
        }
    }

    if (I != -1) {
        goifilt(fmin(round(I + lo + (1. - SIGMA_oqmax) * Tprev), u.size()-1)) = 0.5;
    }
    // If not insert at last OQ.
    else {
        if (prev > 0) {
            goifilt(lo + prev) = 0.5;
        }
    }

    goi = goifilt;

    rGci.resize(0);
    rGoi.resize(0);

    for (int i = 0; i < u.size(); ++i) {
        if (gci(i) > 0.) {
            rGci.push_back(i);
        }
        if (goi(i) > 0.) {
            rGoi.push_back(i);
        }
    }

}

