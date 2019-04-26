#include <algorithm>
#include <array>
#include "audio.h"
#include "gci_yaga_subroutines.h"
#include "print_iter.h"
#include "gci_yaga.h"

 
static constexpr double constLambda[] = {
    0.8, 0.6, 0.4, 0.3, 0.1, 0.5
};

// Threshold for voicing detection
static constexpr double nu = -0.3;

// longest centered windowed segment is 11ms
//static int endSkip;

// how many N-best to keep
static constexpr int Nbest = 3;

// 


void selectCandidates(const valarray& u, const valarray& gamma, candvec& cands, std::vector<int>& bestCands)
{
    static const int endSkip((11. / 1000. * SAMPLE_RATE) / 2 + 2);

    int t;
    
    // skip candidates at each end if necessary (for the cost function to compute properly)
    t = cands.back().first;
    while (t > u.size() - endSkip) {
        cands.pop_back();
        t = cands.back().first;
    }
 
    t = cands.front().first;
    while (t < endSkip) {
        cands.pop_front();
        t = cands.front().first;
    }
    
    if (cands.size() < 1) {
        return;
    }

    valarray norms(cands.size() - 1); 
    double maxNorm;

    // precalculate the norms of u between candidates and drop the unused candidate
    cand_select_precalc_norms(u, cands, norms, maxNorm);
    cands.pop_back();

    const int Ncand(cands.size());

    valarray lambda(constLambda, 6);

    int start, r, q, p, n;

    struct node {
        int t;
        int r, q;
        double cost;
        double cumCost;
    };

    std::array<std::vector<node>, Nbest> paths;
    for (n = 0; n < Nbest; ++n) {
        paths[n].push_back({ cands[2].first, 2, 1, 0., 0. });
    }

    valarray costVector;
    double cost;
   
    int minR = 3;
    int nbCompletePaths = 0;

    // Until all paths are complete
    while (nbCompletePaths < Nbest) {

        // Store the best node of each N
        // Initialize the cost to +inf
        std::array<node, Nbest> bestNode;
        for (n = 0; n < Nbest; ++n) {
            bestNode[n] = { 0, 0, 0, HUGE_VAL, HUGE_VAL };
        }

        // Loop through all subsequent candidates
        for (start = minR; start < Ncand; ++start) {
            r = start;

            // For each best path
            for (n = 0; n < Nbest; ++n) {
                node prevBest = paths[n].back();

                q = prevBest.r;
                p = prevBest.q;

                // Time has to be strictly increasing
                if (q < r) {
                    cost_calc(u, gamma, norms, maxNorm, cands[r], cands[q], cands[p], costVector);
                    
                    // Pass the voicing detector
                    if (costVector[0] < nu) {
                        cost = cost_eval(lambda, costVector);

                        // If this node is lesser than the last least one, replace it.
                        if (cost < bestNode[n].cost) {
                            bestNode[n] = { cands[r].first, r, q, cost, prevBest.cumCost };

                            // Update the bounds for r
                            if (r < minR)
                                minR = r;
                        }
                    }
                }
            }
        }

        nbCompletePaths = 0;

        // Once each new iteration is over, push the best node over on the path.
        // If r = q = 0, that means that path has reached completion.
        for (n = 0; n < Nbest; ++n) {
            node best = bestNode[n];
            if (best.r != 0 && best.q != 0) {
                // Postponed cumulative cost calculation here to save a bit of time
                best.cumCost += best.cost;
                paths[n].push_back(best);
            } else {
                nbCompletePaths++;
            }
        }
    }

    // At the end, pick the path with the smallest cumulative cost
    std::vector<node> bestPath(*std::min_element(paths.begin(), paths.end(),
                [](const auto& a, const auto& b) {
                    return a.back().cumCost < b.back().cumCost;
                }));

    bestCands.resize(bestPath.size());

    std::transform(bestPath.begin(), bestPath.end(), bestCands.begin(),
[](const auto& node) { return node.t; });

    /*
    struct node {
        int m;
        double cost;
    };

    std::array<valarray, Nbest> costs;
    for (int n = 0; n < Nbest; ++n) {
        costs[n].resize(Ncand + 1, HUGE_VAL);
        costs[n][0] = 0.;
    }

    std::array<std::vector<int>, Nbest> paths;

    for (int r = 0; r < Ncand; ++r) {

        int start;
        int m = 2;
    
        while (m < Ncand && cands[r].first >= cands[m].first) {
            m++;
        }
        start = m;
        
        std::vector<node> exhSearch(Ncand);

        for (m = start; m < Ncand; ++m) {
            valarray costs;
            cost_calc(u, gamma, norms, maxNorm, cands[m], cands[m-1], cands[m-2], costs);

            double cost = cost_eval(lambda, costs);            

            exhSearch[m] = { m, cost };
        }

        for (int n = 0; n < Nbest; ++n) {
            // Sort by cumulative cost for each Nbest
            std::sort(exhSearch.begin() + start, exhSearch.end(),
                    [&](node& a, node& b) { return (costs[n][a.m] + a.cost)
                                                    < (costs[n][b.m] + b.cost); });
            
            paths[n].push_back(exhSearch[n].m);

            for (int k = start; k < Ncand; ++k) {
                node node = exhSearch[m];
                costs[n][node.m+1] = costs[n][node.m] + node.cost;
            }
        }
    }

    // At the end, pick the path with the smallest cumulative cost
    int bestPathInd(0);
    double bestCost(0.);

    for (int n = 0; n < Nbest; ++n) {
        double cost = costs[n][paths[n].back()];
        
        if (cost < bestCost) {
            bestCost = cost;
            bestPathInd = n;
        }
    }

    std::vector<int> bestPathNcands = paths[bestPathInd];

    bestCands.resize(bestPathNcands.size());

    std::transform(bestPathNcands.begin(), bestPathNcands.end(), bestCands.begin(),
                    [&](int m) { return cands[m].first; });
    */
}
