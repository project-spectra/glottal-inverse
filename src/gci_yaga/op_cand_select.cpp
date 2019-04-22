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

            Qc[nc][no] = (to - tc) / t0;
        }
    }
    Qc[closed.size()-1] = Qc[closed.size()-2];

    // best path is lowest path of consistent CQ values

    auto costFun = [](double Qcp, double Qcq, double Qcr, double dpq, double dqr, double dpr) {
        return (Qcp + Qcq + Qcr) * (dpq + dqr + dpr) / 6.;
    };

    struct node {
        int t;
        bool found; 
        double cost;    // average CQ times average dist
        double cumCost;
    };

    std::array<vector<node>, Nbest> bestPaths;

    std::array<node, Nbest> best, prev;

    size_t n,p,q,r;
 
    for (n = 0; n < Nbest; ++n) {
        prev[n].t = -1;
        prev[n].cumCost = 0.;
    }

    size_t minP = 0, nextMinP;
    double cost;

    double lastGoodQc = 0.2;

    // Each path needs to have one GOI per cycle
    for (nc = 0; nc < closed.size(); ++nc) {

        for (n = 0; n < Nbest; ++n) {
            best[n].found = false;
            best[n].cost = HUGE_VAL;
        }
      
        nextMinP = SIZE_MAX;

        for (p = minP; p < open.size() - 2; ++p) {
            q = p+1;
            r = p+2;

            // Time is strictly increasing
            if (best[n].t < open[p]) {
                double Qcp, Qcq, Qcr;
                double dpq, dqr, dpr;

                // Get each open quotient and each distance
                Qcp = Qc[nc][p];
                Qcq = Qc[nc][q];
                Qcr = Qc[nc][r];

                dpq = fabs(Qcp - Qcq);
                dqr = fabs(Qcq - Qcr);
                dpr = fabs(Qcp - Qcr);

                // If the CQs are all within xi distance of each other 
                if (dpq <= xi && dqr <= xi && dpr <= xi) {
                    cost = costFun(Qcp, Qcq, Qcr, dpq, dqr, dpr);
                    
                    // If it's a better candidate than the best one so far
                    if (cost < best[n].cost) {
                        best[n] = { static_cast<int>(open[p]), true, cost, prev[n].cumCost };

                        lastGoodQc = Qcp;

                        if (n < nextMinP) {
                            nextMinP = n;
                        }
                    }
                }
            }
        }

        for (n = 0; n < Nbest; ++n) {
            // If no candidate was found, create an "artificial" one
            // from the last good closed quotient
            if (!best[n].found) {
                int tc = closed[n] + lastGoodQc * (closed[nc+1] - closed[n]);
                
                best[n] = { tc, true, lastGoodQc, prev[n].cumCost };
            }

            best[n].cumCost += best[n].cost;
            bestPaths[n].push_back(best[n]);

            prev[n] = best[n];
        }

        minP = nextMinP;
    }

    std::vector<node> bestPath(*std::min_element(bestPaths.begin(), bestPaths.end(),
                    [](const auto& a, const auto& b) { return a.back().cumCost < b.back().cumCost; }));

    std::transform(bestPath.begin(), bestPath.end(), std::back_inserter(bestOpen),
                    [](const node& o) { return o.t; });
}
