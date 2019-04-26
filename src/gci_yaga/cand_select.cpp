#include <algorithm>
#include <array>
#include "audio.h"
#include "gci_yaga_subroutines.h"
#include "gci_yaga.h"

 
static constexpr double constLambda[] = {
    0.8, 0.6, 0.4, 0.3, 0.1, 0.5
};

// Threshold for voicing detection
static constexpr double nu = -0.3;

// longest centered windowed segment is 11ms
//static size_t endSkip;

// how many N-best to keep
static constexpr size_t Nbest = 3;

void selectCandidates(const valarray& u, const valarray& gamma, candvec& cands, std::vector<size_t>& bestCands)
{
    const size_t endSkip = (11. / 1000. * SAMPLE_RATE) / 2 + 2;

    size_t t;
    
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

    valarray norms(cands.size() - 1); 
    double maxNorm;

    size_t start, n;
    size_t r, q, p;

    // precalculate the norms of u between candidates and drop the unused candidate
    cand_select_precalc_norms(u, cands, norms, maxNorm);
    cands.pop_back();

    const size_t Ncand(cands.size());

    valarray lambda(constLambda, 6);

    struct node {
        size_t t;
        size_t r, q;
        double cost;
        double cumCost;
    };

    std::array<std::vector<node>, Nbest> paths;
    for (n = 0; n < Nbest; ++n) {
        paths[n].push_back({ cands[2].first, 2, 1, 0., 0. });
    }

    valarray costVector;
    double cost;
   
    size_t minR = 3;
    size_t nbCompletePaths = 0;

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
}
