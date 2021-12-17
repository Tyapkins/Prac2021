#ifndef TASK3_SIMPLE_H
#define TASK3_SIMPLE_H

#include "graph.h"

class SimpleGraph : public virtual Graph {
public:

    SimpleGraph() = default;
    explicit SimpleGraph(const container& args);
    SimpleGraph(const SimpleGraph& args) = default;
    virtual const std::string ToString() override;

    WeightedGraph operator +(WeightedGraph graph);
};

#endif //TASK3_GRAPH_H
