#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

#include "dijkstra.h"

void dijkstra(const graph& G, const CostFun_t& costFun, int source, int target, std::list<int>& shortestPath, std::map<int, double>& dist)
{
    using node = std::pair<int, double>;

    std::map<int, int> prev;

    // Min-priority queue
    std::vector<node> Q;  // Container
    auto C = [](auto& a, auto& b) { return a.second < b.second; }; // Comparator

    dist[source] = 0.0;

    for (int v : G.vertices) {
        if (v != source) {
            dist[v] = HUGE_VAL;
        }
       
        prev[v] = -1;

        Q[v] = std::make_pair(v, dist[v]);
    }

    std::make_heap(Q.begin(), Q.end(), C);

    while (!Q.empty()) {
        auto [ u, dist_u ] = Q.front();
        std::pop_heap(Q.begin(), Q.end(), C);
        Q.pop_back();

        if (u == target) {
            break;
        }

        // For each neighbour v of u
        for (auto& [ i, v ] : G.edges) {
            if (i != u || v <= u || v >= target)
                continue;

            double alt = dist_u + costFun(prev[u] != -1 ? prev[u] : u, u, v);
            if (alt < dist[v]) {
                dist[v] = alt;
                prev[v] = u;

                auto Qv = std::find_if(Q.begin(), Q.end(), [v = v](auto& node) { return node.first == v; });
                if (Qv != Q.end()) {
                    Qv->second = alt;
                    std::make_heap(Q.begin(), Q.end(), C);
                }
                else {
                    Q.emplace_back(v, alt);
                    std::push_heap(Q.begin(), Q.end(), C);
                }
            }    
        }
    }

    // Traceback shortest path
    shortestPath.resize(0);

    int u = target;

    // Do something only if the vertex is reachable
    if (prev[u] != -1 || u == source) {
        while (u != -1) {
            shortestPath.push_front(u);
            u = prev[u];
        }
    }
}
