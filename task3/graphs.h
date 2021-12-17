#ifndef TASK3_GRAPHS_H
#define TASK3_GRAPHS_H

#include <memory>
#include "graphs/graph.h"
#include "graphs/simple.h"
#include "graphs/complete.h"
#include "graphs/bipartite.h"
#include "graphs/weighted.h"

template<typename T>
WeightedGraph WeightedGraph::operator-(T graph) {
    auto sum = name_weights - graph.AsWeighted(0).name_weights;
    auto k_and_v = map_to_vectors(sum);
    return WeightedGraph({{k_and_v.first}, {k_and_v.second}});
}

template<typename T>
WeightedGraph WeightedGraph::operator+(T graph) {
    throw std::logic_error("Could not provide sum: second term is not graph or its edges have no weights");
}

template <typename T, typename K> requires (not std::same_as<T, WeightedGraph>)
SimpleGraph operator+(T first, K second)
{
    if (not std::same_as<K, WeightedGraph>)
        return SimpleGraph({first.GetEdges()+second.GetEdges()});
    else
        throw std::logic_error("Could not provide sum: first graph edges have no weights");
}

template <typename T, typename K> requires (not std::same_as<T, WeightedGraph>)
SimpleGraph operator-(T first, K second)
{
    return SimpleGraph({first.GetEdges()-second.GetEdges()});
}

template <typename T, typename K>
T operator += (T& one, K& other)
{
    if ((std::same_as<T, K>) or (std::same_as<T, SimpleGraph>) or (std::same_as<T, WeightedGraph>) or (std::same_as<K, WeightedGraph>)) {
        auto new_gr = one + other;
        one.CopyGraph(new_gr);
    }
    else
        throw std::logic_error("Can not change first term type through operation");
    return one;
}

template <typename T, typename K>
T operator -= (T& one, K& other)
{
    if ((std::same_as<T, K>) or (std::same_as<T, SimpleGraph>) or (std::same_as<T, WeightedGraph>)) {
        auto new_gr = one - other;
        one.CopyGraph(new_gr);
    }
    else
        throw std::logic_error("Can not change first term type through operation");
    return one;
}

#endif //TASK3_GRAPHS_H
