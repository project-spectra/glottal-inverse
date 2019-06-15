#include <algorithm>
#include <array>
#include <map>
#include <tuple>
#include <iterator>
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
    static const int searchLength = 1.1 * T0mean * SAMPLE_RATE;

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

    for (int i = 0; i < Ncand; ++i) {
        G.vertices.emplace(i);

        for (int j = i + 1; j < Ncand; ++j) {
            if (cands[j].first - cands[i].first > searchLength)
                break;

            G.edges.emplace(i, j);
        }
    }

    std::list<int> bestPath;
    double bestCost = -HUGE_VAL;

    std::list<int> path;
    std::map<int, double> dist;
    
    int source, target;

    source = 2;

    while (source < Ncand && cands[source].first - cands[2].first <= searchLength)
    {
        target = Ncand - 1;

        while (source < target && cands[Ncand - 1].first - cands[target].first <= searchLength)
        { 
            path.clear();
            dist.clear();

            dijkstra(G, costFun, source, target, path, dist);

            std::cout << "path(" << source << " -> " << target << ") = [";
            std::copy(path.begin(), path.end(), std::ostream_iterator<int>(std::cout, " -> "));
            std::cout << "]" << std::endl;

            double cost = dist[target];

            if (cost > bestCost) {
                bestPath = path;
                bestCost = cost;
            }

            target--;
        }

        source++;
    }

    bestCands.resize(bestPath.size());

    std::transform(
        bestPath.begin(),
        bestPath.end(),
        bestCands.begin(),
        [&cands](auto i) { return cands[i].first; }
    );

}
