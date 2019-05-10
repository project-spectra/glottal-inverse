#include <array>
#include <set>
#include <utility>
#include <vector>

#include "dijkstra.h"


template<int K>
void kShortestPaths(graph& G, const CostFun_t& costFun, int source, int target, std::array<std::list<int>, K>& A)
{
    struct path {
        std::list<int> path;
        std::map<int, double> dist;
    };

    // Cost vector of each kth path.
    std::array<std::map<int, double>, K> dists;

    // Find the shortest path from source to target
    dijkstra(G, costFun, source, target, A[0], dists[0]);

    // Initialize the set to store the potential kth shortest path.
    std::vector<path> B;

    for (int k = 1; k < K; ++k) {
        auto& prevA = A[k - 1];
        auto& prevDist = dists[k - 1];

        B.resize(0);

        // The spur node ranges from the first node to next to last node in the previous k-shortest path.
        auto spurIt = prevA.begin();
        for (int i = 0; i < prevA.size() - 2; ++i) {
            // Save edges and nodes to restore them later
            auto oldEdges(G.edges);
            auto oldNodes(G.vertices);
            
            // Spur node is retrieved from the previous k-shortest path.
            std::list<int> rootPath;
            rootPath.insert(rootPath.end(), prevA.begin(), spurIt);

            int spurNode = rootPath.back();

            auto [ iStart, iEnd ] = G.edges.equal_range(spurNode);
            
            for (auto& pathList : A) {
                std::vector<int> path;
                std::copy(pathList.begin(), pathList.end(), std::back_inserter(path));

                // If rootPath = path[0..i]
                if (std::equal(rootPath.begin(), rootPath.end(),
                               path.begin(), path.begin() + i)) {
                    // Remove the links that are part of the previous
                    // shortest paths which share the same root path
                    for (auto it = iStart; it != iEnd; ++it) {
                        if (it->second == path[i + 1]) {
                            G.edges.erase(it);
                            break;
                        }
                    }
                }
            }

            std::list<int> spurPath;
            std::map<int, double> spurDist;
            dijkstra(G, costFun, spurNode, target, spurPath, spurDist);

            rootPath.splice(rootPath.end(), spurPath);

            // Add the cost of the root path
            double spurLastDist = prevDist[spurNode];
            for (auto& [ v, d ] : spurDist) {
                spurDist[v] = spurLastDist + d;
            }

            // Merge root dist and spur dist
            std::map<int, double> totalDist;
            std::merge(prevDist.begin(), prevDist.end(),
                       spurDist.begin(), spurDist.end(),
                       std::inserter(totalDist, totalDist.begin()));

            path p = { rootPath, totalDist };
            B.emplace_back(p);
            
            G.edges = oldEdges;
            G.vertices = oldNodes;

            ++spurIt;
        }

        if (B.empty()) {
            // This handles the case of there being no spur path, or no spur paths left.
            // This could happen if the spur paths have already been exhausted (added to A),
            // or there are no spur paths at all - such as when both the source and target vertices
            // lie along a "dead end".
            break;
        }

        // Sort the potential k-shortest paths by cost
        std::sort(B.begin(), B.end(), [=](auto& a, auto& b) {
                                        return a.dist[target] < b.dist[target];
                                      });

        // Add the lowest cost path, as the k-shortest path.
        path ks = B[0];
        A[k] = ks.path;
        dists[k] = ks.dist;
    }
}


