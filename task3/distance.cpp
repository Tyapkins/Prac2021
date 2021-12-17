#include <functional>
#include "distance.h"

bool is_connected(Graph& graph)
{
    auto all_verts = graph.GetVertices();
    auto all_edges = graph.GetEdges();
    std::set<edge> edges_to_find(all_edges.begin(), all_edges.end());
    if (all_verts.empty())
        return true;
    auto next_el = *all_verts.begin();
    vertices visited, newly_visited = {next_el};
    while (!newly_visited.empty())
    {
        vertices not_visited;
        for (const auto& el : newly_visited)
        {
            std::set<edge> edges_to_delete;
            visited.insert(el);
            for (const auto& new_edge: edges_to_find)
            {
                name second_vert = (new_edge.val.first == el) ? (new_edge.val.second) :
                        (((new_edge.val.second == el)) ? (new_edge.val.first) : "");
                if (!(second_vert.empty()) and (!visited.contains(second_vert))) {
                    not_visited.insert(second_vert);
                    edges_to_delete.insert(new_edge);
                }
            }
            for (const auto& del_el : edges_to_delete)
                edges_to_find.erase(del_el);
        }
        newly_visited.clear();
        for (const auto& add_el : not_visited) {
            newly_visited.insert(add_el);
        }
    }
    return visited == all_verts;
}

vertices get_neighbours(Graph& graph, const name& vertice)
{
    edges chosen_edges, all_edges = graph.GetEdges();
    vertices neighbours;
    std::copy_if(all_edges.begin(), all_edges.end(), std::back_inserter(chosen_edges),
                 [vertice](const edge& num){return ((num.val.first==vertice)
                 or (num.val.second==vertice));});
    std::transform(chosen_edges.begin(), chosen_edges.end(), std::inserter(neighbours, neighbours.begin()),
                   [vertice](const edge& num){return (num.val.first == vertice) ? num.val.second : num.val.first;});
    return neighbours;
}


edges shortest_way(WeightedGraph graph, const name& first, const name& second)
{
    if (not(is_connected(graph)))
        throw std::logic_error("Graph should be connected!");
    std::map<name, weight> vert_weights;
    auto all_verts = graph.GetVertices();
    auto all_edges = graph.GetEdges();
    auto edges_and_weights = graph.GetWeights();
    vertices not_visited(all_verts.begin(), all_verts.end());
    std::transform(all_verts.begin(), all_verts.end(),
                   std::inserter(vert_weights, vert_weights.begin()),
                   [](name n) { return std::make_pair<name, weight>(std::forward<name>(n), UINTMAX_MAX); });
    vert_weights[first] = 0;
    std::map<name, weight> remained_verts(vert_weights.begin(), vert_weights.end());
    while (!not_visited.empty())
    {
        auto min_vert_weight = std::min_element(remained_verts.begin(), remained_verts.end(),
                                                [](const auto& l, const auto& r) { return (l.second < r.second); });
        auto min_vert = min_vert_weight->first;
        not_visited.erase(min_vert);
        remained_verts.erase(min_vert);
        edges chosen_edges;
        vertices neighbours = get_neighbours(graph, min_vert);
        for (const auto& el : neighbours) {
            vert_weights[el] = std::min(vert_weights[el], vert_weights[min_vert] +
            std::max(edges_and_weights[{{min_vert, el}}], edges_and_weights[{{el, min_vert}}]));
            if (remained_verts.contains(el))
                remained_verts[el] = vert_weights[el];
        }
    }
    edges path;
    name cur_vert = second;
    while (cur_vert != first)
    {
        auto cur_neighb = get_neighbours(graph, cur_vert);
        for (const auto& next_vert : cur_neighb)
        {

            if (vert_weights[cur_vert] - vert_weights[next_vert] ==
            std::max(edges_and_weights[{{cur_vert, next_vert}}], edges_and_weights[{{next_vert, cur_vert}}]))
            {
                path.insert(path.begin(), {{next_vert, cur_vert}});
                cur_vert = next_vert;
                break;
            }
        }
    }
    return path;
}
