#include <armadillo>
#include <vector>

#include "audio.h"
#include "dyprog_lf.h"

using namespace arma;

void dyprof_lf(const valarray& dg, std::vector<int>& GCIs)
{
    // Maximum voiced frequency 
    static constexpr double MVF = 2500.;

    // Dynamic programming weights
    static constexpr double w_time = 0.1;
    static constexpr double w_freq = 0.3;
    static constexpr double w_trans = 0.3;

    // Allocated space
    struct params { double F0, EE, Ra, Rk, Eg, UP; };
    std::vector<params> params(GCIs.size(), { 0., 0., 0., 0., 0., 0. });

    // Optimisation parameters
    static constexpr double optiTol = 1e-10;
    static constexpr int maxEvals = 20;

    dvec EE = zeros<dvec>(GCIs.size());
    dvec Rd_set = regspace<dvec>(0.3, 0.17, 2);
    int pulseNum = 2;

    // Dynamic programming settings
    int nframe = GCIs.size();
    int ncands = 5;
    dmat Rd_n = zeros<dmat>(nframe, ncands);
    dmat cost = zeros<dmat>(nframe, ncands); // Cumulative cost
    dmat prev = zeros<dmat>(nframe, ncands); // Traceback pointer

    // Align GCIs to main excitations
    correct_GCI_glot(dg, GCIs);

    

}
