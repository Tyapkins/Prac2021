#ifndef TASK2_GRAPH_H
#define TASK2_GRAPH_H

#include "parsing.h"
#include "con_component.h"

class Graph : public con_component
{
    size_t node_num = 0;
    full_graph components;

public:

    Graph() = default;

    [[maybe_unused]] explicit Graph(size_t nodes) : node_num(nodes) {};

    [[maybe_unused]] explicit Graph(full_graph comps) : components(std::move(comps)) {};
    Graph(size_t nodes, full_graph comps) : node_num(nodes), components(std::move(comps)) {};

    static std::string num_to_name(std::size_t num, u_char first_sym, std::size_t alph_size);

    full_graph get_graph() {return components;}

    [[maybe_unused]] void to_dot(const std::string& filename);
    jobList get_all_jobs();
    void to_out_file(const std::string& filename);
};

Graph generate_graph(size_t node_num,
                     interval weights, double weight_disp,
                     parameters con_components_par,
                     parameters degree_par);


#endif //TASK2_GRAPH_H
