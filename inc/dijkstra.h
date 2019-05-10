#ifndef DIJKSTRA_H
#define DIJKSTRA_H


#include <functional>
#include <list>
#include <map>
#include <set>
#include <utility>

using CostFun_t = std::function<double(int, int, int)>;

struct graph {
    std::set<int> vertices;
    std::multimap<int, int> edges;
};

void dijkstra(const graph& G, const CostFun_t& costFun, int source, int target, std::list<int>& shortestPath, std::map<int, double>& dists);


#include "dijkstra_impl.h"


#endif // DIJKSTRA_H
