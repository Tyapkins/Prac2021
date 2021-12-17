#ifndef TASK3_BIPARTITE_H
#define TASK3_BIPARTITE_H

#include "graph.h"
#include "simple.h"

class BipartiteGraph : public virtual Graph, public virtual SimpleGraph  {

    vertices second_verts;

    edges GetEdgesByVerts();

public:

    explicit BipartiteGraph(const container& args);
    const std::string ToString() override;

    std::pair<vertices, vertices> GetVerticesByParts() {return {Graph::GetVertices(), second_verts};}
    void SetVerticesByParts(const std::pair<vertices, vertices>& vert_part)
    {
        SetVertices(vert_part.first);
        SetSecondVertices(vert_part.second);
    }
    vertices GetVertices() override;
    vertices GetSecondPart() {return second_verts;}

    void SetSecondVertices(vertices other_verts) {second_verts = std::move(other_verts);}

    void CopyGraph(BipartiteGraph& other_graph);

    BipartiteGraph operator +(BipartiteGraph second);

    BipartiteGraph operator -(BipartiteGraph second);
};

#endif //TASK3_BIPARTITE_H
