#include "complete.h"

CompleteGraph::CompleteGraph(const container& args) : Graph({&typeid(vertices)}, args) {
    SetVertices(std::any_cast<vertices>(args.at(0)));
    SetEdges(GetEdgesByVerts());
}

const std::string CompleteGraph::ToString() {
    return "CompleteGraph " + VerticesToString();
}

edges CompleteGraph::GetEdgesByVerts() {
    edges all_edges;
    auto verts = GetVertices();
    for (auto first_it = verts.begin(); first_it != verts.end(); first_it++)
        for (auto second_it = first_it; second_it != verts.end(); second_it++)
            if (*first_it != *second_it)
                all_edges.push_back({{*first_it, *second_it}});
    return all_edges;
}

CompleteGraph CompleteGraph::operator+(CompleteGraph second) {
    return CompleteGraph({GetVertices() + second.GetVertices()});
}

CompleteGraph CompleteGraph::operator-(CompleteGraph second) {
    return CompleteGraph({GetVertices() - second.GetVertices()});
}

