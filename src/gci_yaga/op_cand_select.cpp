#include <algorithm>
#include <array>
#include "gci_yaga.h"


using std::vector;
using std::deque;


static constexpr size_t Nbest = 3;

static constexpr double xi = 0.1;

void selectOpenCandidates(const vector<size_t>& closed, const deque<size_t>& open, vector<size_t>& bestOpen) {

    // row: nc, column: no
    std::valarray<valarray> Qc(closed.size());

    size_t nc, no;

    for (nc = 0; nc < closed.size() - 1; ++nc) {
        double tc = closed[nc];
        double t0 = closed[nc+1] - tc; 

        Qc[nc].resize(open.size());
        for (no = 0; no < open.size(); ++no) {
            double to = open[no];

            Qc[nc][no] = fmin(fmax((to - tc) / t0, 0.), 1.);
        }
    }
    Qc[closed.size()-1] = Qc[closed.size()-2];

    // best path is lowest path of consistent CQ values

    auto costFun = [](double Qcp, double Qcq, double Qcr, double dpq, double dqr) {
        return (Qcp + Qcq + Qcr) * (dpq + dqr) / 5.;
    };

    struct node {
        size_t p,q,r; // starts from p, p+1, p+2
        double cost;  // average CQ times average dist
    };

    std::array<vector<node>, Nbest> bestPaths;

    std::array<node, Nbest> best, prev;

    const size_t Ncand(open.size());
    size_t n,p,q,r;
 
    for (n = 0; n < Nbest; ++n) {
        best[n].r = 0;
        prev[n].cost = HUGE_VAL;
    }

    size_t minP = 0;
    size_t nbCompletePaths = 0;

    while (nbCompletePaths < Nbest) {

        for (n = 0; n < Nbest; ++n) {
            prev[n] = 
        }
    }
    

}
