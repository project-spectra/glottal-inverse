#include <algorithm>
#include <array>
#include <map>
#include <tuple>
#include "audio.h"
#include "gci_yaga_subroutines.h"
#include "print_iter.h"
#include "k_shortest.h"
#include "gci_yaga.h"

 
static constexpr double constLambda[] = {
    0.8, 0.6, 0.4, 0.3, 0.1, 0.5
};

// Threshold for voicing detection
static constexpr double nu = -0.3;

// longest centered windowed segment is 11ms
//static int endSkip;

// how many N-best to keep
static constexpr int Nbest = 1;

// 


void selectCandidates(const valarray& u, const valarray& gamma, double T0mean, candvec& cands, std::vector<int>& bestCands)
{
    static const int endSkip((11. / 1000. * SAMPLE_RATE) / 2 + 2);
  
    static const int searchLength = 1.3 * T0mean * SAMPLE_RATE;

    int i, t;
    
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

    // precalculate the norms of u between candidates and drop the unused candidate
    valarray norms(cands.size() - 1); 
    double maxNorm;

    cand_select_precalc_norms(u, cands, norms, maxNorm);
    cands.pop_back();

    // precalculate the FN of each sample
    valarray FNs(u.size());
    double maxFN;
    cand_select_precalc_frobs(u, FNs, maxFN);

    const int Ncand(cands.size());

    std::map<std::tuple<int, int, int>, double> costs;

    const valarray lambda(constLambda, 6);
    valarray costVector;
    double cost;

    auto costFun = [&](int k, int j, int i) -> double {
        auto t = std::make_tuple(i, j, k);
        auto it = costs.find(t);

        if (it != costs.end()) {
            return it->second;
        }
        else {
            cost_calc(
                u, gamma, norms, maxNorm, FNs, maxFN,
                cands[i], cands[j], cands[k],
                costVector
            );
    
            cost = cost_eval(lambda, costVector);
            
            costs[t] = cost;
            return cost;
        }
    };

    graph G;

    for (i = 0; i < Ncand; ++i) {
        G.vertices.emplace(i);
    }

    std::map<int, std::list<int>> paths; // start -> bestPath
    std::map<int, double> dists;         // start -> cost

    // For each expected cycle (with some margin), find the best path from the previous to the current cycles.
    int t1, ts;
        
    // We have to initialize the first 3 cycles blindly, so we'll do an exhaustive search.
    i = 0;
    t1 = cands[i].first;
    ts = cands[i + 1].first;

    while (ts - t1 < 3 * searchLength) {
        G.edges.erase(G.edges.begin(), G.edges.end());

        G.edges.emplace(ts, t1);

    }

    //dijkstra(G, costFun, 2, Ncand - 1, paths);
    

}
