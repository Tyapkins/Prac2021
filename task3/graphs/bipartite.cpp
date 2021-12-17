#include "bipartite.h"

BipartiteGraph::BipartiteGraph(const container& args) : Graph({&typeid(vertices), &typeid(vertices)}, args) {
    auto first_part = std::any_cast<vertices>(args.at(0));
    auto second_part = std::any_cast<vertices>(args.at(1));
    vertices intersect;
    std::set_intersection(first_part.begin(), first_part.end(),
                          second_part.begin(), second_part.end(),
                          std::inserter(intersect, intersect.begin()));
    if (!intersect.empty())
        throw std::invalid_argument("Bipartite Graph vertices of two parts should not intersect");
    SetVertices(first_part);
    SetSecondVertices(second_part);
    SetEdges(GetEdgesByVerts());
}

const std::string BipartiteGraph::ToString() {
    return "BipartiteGraph " + VerticesToString() + ", " + VectorToString(second_verts);
}

edges BipartiteGraph::GetEdgesByVerts() {
    edges all_edges;
    for (const auto& el_f : Graph::GetVertices())
        for (const auto& el_s : second_verts)
            all_edges.push_back({{el_f, el_s}});
    return all_edges;
}

vertices BipartiteGraph::GetVertices() {
    return Graph::GetVertices() + second_verts;
}

BipartiteGraph BipartiteGraph::operator+(BipartiteGraph second) {
    return BipartiteGraph({{Graph::GetVertices() + second.Graph::GetVertices()},
                          {second_verts + second.GetSecondPart()}});
}

BipartiteGraph BipartiteGraph::operator-(BipartiteGraph second) {
    return BipartiteGraph({{Graph::GetVertices() - second.Graph::GetVertices()},
                           {second_verts - second.GetSecondPart()}});
}

void BipartiteGraph::CopyGraph(BipartiteGraph &other_graph) {
    SetEdges(other_graph.GetEdges());
    SetVerticesByParts(other_graph.GetVerticesByParts());
}
