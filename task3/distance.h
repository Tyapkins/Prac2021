#ifndef TASK3_DISTANCE_H
#define TASK3_DISTANCE_H

#include "funcs.h"
#include "graphs/simple.h"
#include "graphs/weighted.h"

bool is_connected(Graph& graph);

vertices get_neighbours(Graph& graph, const name& vertice);

edges shortest_way(WeightedGraph graph, const name& first, const name& second);

#endif //TASK3_DISTANCE_H
