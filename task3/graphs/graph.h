#ifndef TASK3_GRAPH_H
#define TASK3_GRAPH_H

#include <set>
#include <utility>
#include <vector>
#include <any>
#include <string>
#include <tuple>
#include <iostream>
#include <map>
#include "types.h"

class SimpleGraph;
class WeightedGraph;

class Graph{
    vertices verts;
    edges edgs;
    type_vector req_types;

public:

    Graph() = default;
    explicit Graph(type_vector  vects, const container& args);

    virtual vertices GetVertices() {return verts;}
    void SetVertices(vertices new_verts) {verts = std::move(new_verts);}

    vertices GetVertsByEdges();

    edges GetEdges() {return edgs;}
    void SetEdges(edges new_edg) {edgs = std::move(new_edg);}

    virtual const std::string ToString() = 0;

    template <typename T>
    std::string static VectorToString(T vec);

    std::string VerticesToString();
    std::string EdgesToString();

    virtual WeightedGraph AsWeighted(int default_weight);

    void CopyGraph(Graph& other_graph);
};

template <typename ForwardIterator>
ForwardIterator remove_duplicates( ForwardIterator first, ForwardIterator last );


template <typename... Args>
container mas_to_container(container mas, Args... args);

template <typename First, typename... Args>
container mas_to_container(container& mas, First next_arg, Args... args);

vertices operator+ (const vertices& first, const vertices& second);
edges operator+ (edges first, const edges& second);

vertices operator- (const vertices& first, const vertices& second);
edges operator- (edges first, const edges& second);

#endif //TASK3_GRAPH_H
