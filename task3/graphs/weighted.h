#ifndef TASK3_WEIGHTED_H
#define TASK3_WEIGHTED_H

#include "graph.h"
#include "simple.h"


class WeightedGraph : public virtual Graph, public virtual SimpleGraph {

public:
    struct weight_pair {std::pair<edge, weight> val;
        explicit operator std::string() const {return std::string(val.first) + ": " + std::to_string(val.second);}
    };

    using name_with_weights = std::map<edge, weight>;

private:

    weights vert_weights;
    name_with_weights name_weights;

public:

    WeightedGraph() = default;

    explicit WeightedGraph(const container& args);
    const std::string ToString() override;

    void SetWeights(weights new_wts) {vert_weights = std::move(new_wts);}
    WeightedGraph AsWeighted(int num) override {return *this;}

    WeightedGraph operator +(const WeightedGraph& second);

    name_with_weights GetWeights() {return name_weights;}

    template <typename T>
    WeightedGraph operator +(T gr);

    template <typename T>
    WeightedGraph operator -(T gr);

};

using wpair = WeightedGraph::weight_pair;
using n_wpair = WeightedGraph::name_with_weights;

wpair operator+(const wpair& first, const wpair& second);
n_wpair operator+ (const n_wpair& first, n_wpair second);
n_wpair operator-(const n_wpair& first, const n_wpair& second);

#endif //TASK3_WEIGHTED_H
