#include "funcs.h"
#include "weighted.h"


WeightedGraph::WeightedGraph(const container& args) : Graph({&typeid(edges), &typeid(weights)}, args), SimpleGraph(args) {
    auto second_arg = std::any_cast<weights>(args.at(1));
    auto edg_names = GetEdges();
    if (edg_names.size() != second_arg.size())
        throw std::invalid_argument("Invalid argument: number of weights and edges should be the same");
    SetWeights(second_arg);
    for (int i = 0; i < edg_names.size(); i++)
        name_weights.emplace(edg_names[i], vert_weights[i]);
}

const std::string WeightedGraph::ToString() {
    return "WeightedGraph " + VectorToString(std::vector<weight_pair>(name_weights.begin(), name_weights.end()));
}

WeightedGraph WeightedGraph::operator+(const WeightedGraph& second) {
    auto sum = name_weights + second.name_weights;
    auto k_and_v = map_to_vectors(sum);
    return WeightedGraph({k_and_v.first, k_and_v.second});
}

n_wpair operator+(const n_wpair& first, n_wpair second)
{
    n_wpair res;
    for (const auto& el: first)
        res.insert({el.first, second.contains(el.first) ? std::min(el.second, second[el.first]): el.second});
    for (const auto& el: second)
        if (!res.contains(el.first))
            res.insert({el.first, el.second});
    return res;
}

n_wpair operator-(const n_wpair& first, const n_wpair& second)
{
    n_wpair res(first.begin(), first.end());
    for (const auto& el: second)
        if (res.contains(el.first))
            res.erase(el.first);
    return res;
}

