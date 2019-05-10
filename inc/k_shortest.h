#ifndef K_SHORTEST_H
#define K_SHORTEST_H


#include <array>
#include <list>
#include "dijkstra.h"


template<int K>
void kShortestPaths(graph& G, const CostFun_t& costFun, int source, int target, std::array<std::list<int>, K>& shortest);


#include "k_shortest_impl.h"


#endif // K_SHORTEST_H
