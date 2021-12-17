#include "graph.h"
#include "weighted.h"
#include "funcs.h"

bool edge::operator==(const edge &other) const {
    return  ((val.first == other.val.first) && (val.second == other.val.second))
            ||
            ((val.first == other.val.second) && (val.second == other.val.first));
}

bool edge::operator<(const edge &other) const {
    return (val.first.compare(other.val.first) < 0) || ((val.first == other.val.first) && (val.second.compare(other.val.second) < 0));
}


Graph::Graph(type_vector vects, const container &args) :  req_types(std::move(vects)) {
    auto req_size = req_types.size();
    if (args.size() != req_size)
        throw std::invalid_argument("Invalid number of arguments");
    int i = 0;
    for (const auto& arg : args)
    {
        if (*req_types.at(i) != arg.type())
            throw std::invalid_argument("Wrong argument type at position " + std::to_string(i));
        i++;
    }
}

std::string Graph::VerticesToString()
{
    return VectorToString(verts);
}

std::string Graph::EdgesToString() {
    return VectorToString(edgs);
}

vertices Graph::GetVertsByEdges() {
    vertices all_verts;
    for (const auto &ed : edgs) {
        if (std::find(all_verts.begin(), all_verts.end(), ed.val.first) == all_verts.end())
            all_verts.insert(ed.val.first);
        if (std::find(all_verts.begin(), all_verts.end(), ed.val.second) == all_verts.end())
            all_verts.insert(ed.val.second);
    }
    return all_verts;
}

WeightedGraph Graph::AsWeighted(int default_weight) {
    auto all_edgs = GetEdges();
    return WeightedGraph({all_edgs, std::vector<weight>(all_edgs.size(), default_weight)});
}

void Graph::CopyGraph(Graph &other_graph) {
    SetVertices(other_graph.GetVertices());
    SetEdges(other_graph.GetEdges());
}

vertices operator+ (const vertices& first, const vertices& second) {
    vertices res;
    std::set_union(first.begin(), first.end(),
                   second.begin(), second.end(),
                   std::inserter(res, res.begin()));
    return res;
}

vertices operator- (const vertices& first, const vertices& second) {
    vertices res;
    std::set_difference(first.begin(), first.end(),
                   second.begin(), second.end(),
                   std::inserter(res, res.begin()));
    return res;
}

edges operator+ (edges first, const edges& second)
{
    edges res(first.begin(), first.end());
    for (const auto& el : second)
        if (std::find(res.begin(), res.end(), el) == res.end())
            res.push_back(el);
    return res;
}

edges operator- (edges first, const edges& second)
{
    edges res(first.begin(), first.end());
    for (const auto& el : second) {
        auto found_el = std::find(res.begin(), res.end(), el);
        if (found_el != res.end())
            res.erase(found_el);
    }
    return res;
}
