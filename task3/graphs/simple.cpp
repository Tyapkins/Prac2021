#include "funcs.h"
#include "simple.h"
#include "weighted.h"

SimpleGraph::SimpleGraph(const container& args) : Graph({&typeid(edges)}, args) {
    auto all_edges = std::any_cast<edges>(args.at(0));
    all_edges.erase(remove_duplicates(all_edges.begin(), all_edges.end()), all_edges.end());
    SetEdges(all_edges);
    SetVertices(GetVertsByEdges());
}

const std::string SimpleGraph::ToString() {
    return "SimpleGraph " + EdgesToString();
}

WeightedGraph SimpleGraph::operator+(WeightedGraph graph) {
    throw std::logic_error("Could not provide sum: first graph edges have no weights");
}