#include <gtest/gtest.h>
#include "factory.h"
#include "funcs.h"

class GraphsTest : public ::testing::Test {
protected:
    virtual void SetUp() {

        TFactory new_fac;

        simple_one = {{{"S1", "A"}}, {{"S1", "B"}}, {{"A", "B"}}, {{"B", "D"}}, {{"E", "D"}}};
        simple_two = {{{"S2", "A"}}, {{"S2", "D"}}, {{"A", "B"}}, {{"B", "D"}}, {{"E", "D"}}};

        complete_one = {"C1", "A", "B", "D", "F"}, complete_two = {"C2", "A", "B", "D", "F"};

        first_bip_one = {"P1", "A", "D"}, first_bip_two = {"P2", "A", "D"};
        second_bip_one = {"B", "T1", "G", "J"}, second_bip_two = {"B", "T2", "G", "J"};

        weight_edg_one = {{{"W1", "A"}}, {{"W1", "K"}}, {{"A", "B"}}, {{"B", "K"}}, {{"K", "D"}}};
        weight_edg_two = {{{"W2", "A"}}, {{"W2", "D"}}, {{"A", "B"}}, {{"B", "K"}}, {{"K", "D"}}};
        weight_one = {5, 10, 15, 20, 25};
        weight_two = {25, 20, 15, 10, 30};

        first_simple = std::dynamic_pointer_cast<SimpleGraph>(new_fac.CreateObject("simple", simple_one));
        second_simple = std::dynamic_pointer_cast<SimpleGraph>(new_fac.CreateObject("simple", simple_two));

        first_complete = std::dynamic_pointer_cast<CompleteGraph>(new_fac.CreateObject("complete", complete_one));
        second_complete = std::dynamic_pointer_cast<CompleteGraph>(new_fac.CreateObject("complete", complete_two));

        first_bipartite = std::dynamic_pointer_cast<BipartiteGraph>(new_fac.CreateObject("bipartite", first_bip_one, second_bip_one));
        second_bipartite = std::dynamic_pointer_cast<BipartiteGraph>(new_fac.CreateObject("bipartite", first_bip_two, second_bip_two));

        first_weighted = std::dynamic_pointer_cast<WeightedGraph>(new_fac.CreateObject("weighted", weight_edg_one, weight_one));
        second_weighted = std::dynamic_pointer_cast<WeightedGraph>(new_fac.CreateObject("weighted", weight_edg_two, weight_two));
    }

    edges simple_one, simple_two, weight_edg_one, weight_edg_two;
    vertices complete_one, complete_two, first_bip_one, first_bip_two, second_bip_one, second_bip_two;
    weights weight_one, weight_two;

    std::shared_ptr<SimpleGraph> first_simple, second_simple;
    std::shared_ptr<CompleteGraph> first_complete, second_complete;
    std::shared_ptr<BipartiteGraph> first_bipartite, second_bipartite;
    std::shared_ptr<WeightedGraph> first_weighted, second_weighted;
};

TEST_F(GraphsTest, StringTest)
{
    ASSERT_EQ("SimpleGraph {S1-A, S1-B, A-B, B-D, E-D}", first_simple->ToString());
    ASSERT_EQ("SimpleGraph {S2-A, S2-D, A-B, B-D, E-D}", second_simple->ToString());

    ASSERT_EQ("CompleteGraph {A, B, C1, D, F}", first_complete->ToString());
    ASSERT_EQ("CompleteGraph {A, B, C2, D, F}", second_complete->ToString());

    ASSERT_EQ("BipartiteGraph {A, D, P1}, {B, G, J, T1}", first_bipartite->ToString());
    ASSERT_EQ("BipartiteGraph {A, D, P2}, {B, G, J, T2}", second_bipartite->ToString());

    ASSERT_EQ("WeightedGraph {A-B: 15, B-K: 20, K-D: 25, W1-A: 5, W1-K: 10}", first_weighted->ToString());
    ASSERT_EQ("WeightedGraph {A-B: 15, B-K: 10, K-D: 30, W2-A: 25, W2-D: 20}", second_weighted->ToString());
}

TEST_F(GraphsTest, EdgesTest)
{
    auto simple_one_edges = first_simple->GetEdges(), simple_two_edges = second_simple->GetEdges();
    auto complete_one_edges = first_complete->GetEdges(), complete_two_edges = second_complete->GetEdges();
    auto bipartite_one_edges = first_bipartite->GetEdges(), bipartite_two_edges = second_bipartite->GetEdges();
    auto weighted_one_edges = first_weighted->GetEdges(), weighted_two_edges = second_weighted->GetEdges();

    ASSERT_EQ(simple_one_edges, simple_one);
    ASSERT_EQ(simple_two_edges, simple_two);

    std::set<edge> first_comp, second_comp;

    for (const auto& el : complete_one)
        for (const auto& other_el : complete_one)
            if (el < other_el)
                first_comp.insert({{el, other_el}});

    for (const auto& el : complete_two)
        for (const auto& other_el : complete_two)
            if (el < other_el)
                second_comp.insert({{el, other_el}});

    ASSERT_EQ(std::set<edge>(complete_one_edges.begin(), complete_one_edges.end()),
              first_comp);

    ASSERT_EQ(std::set<edge>(complete_two_edges.begin(), complete_two_edges.end()),
              second_comp);

    std::set<edge> first_bip, second_bip;

    for (const auto& el : first_bip_one)
        for (const auto& other_el : second_bip_one)
            first_bip.insert({{el, other_el}});

    for (const auto& el : first_bip_two)
        for (const auto& other_el : second_bip_two)
            second_bip.insert({{el, other_el}});

    ASSERT_EQ(std::set<edge>(bipartite_one_edges.begin(), bipartite_one_edges.end()),
              first_bip);
    ASSERT_EQ(std::set<edge>(bipartite_two_edges.begin(), bipartite_two_edges.end()),
              second_bip);


    ASSERT_EQ(weighted_one_edges, weight_edg_one);
    ASSERT_EQ(weighted_two_edges, weight_edg_two);
}


TEST_F(GraphsTest, VerticesTest)
{
    auto simple_one_verts = first_simple->GetVertices(), simple_two_verts = second_simple->GetVertices();
    auto complete_one_verts = first_complete->GetVertices(), complete_two_verts = second_complete->GetVertices();
    auto bipartite_one_verts = first_bipartite->GetVertices(), bipartite_two_verts = second_bipartite->GetVertices();
    auto weighted_one_verts = first_weighted->GetVertices(), weighted_two_verts = second_weighted->GetVertices();

    vertices check = {"S1", "A", "B", "D", "E"};
    ASSERT_EQ(simple_one_verts, check);

    check = {"S2", "A", "B", "D", "E"};
    ASSERT_EQ(simple_two_verts, check);

    ASSERT_EQ(complete_one_verts, complete_one);
    ASSERT_EQ(complete_two_verts, complete_two);

    check = {"P1", "A", "D", "B", "T1", "G", "J"};
    ASSERT_EQ(bipartite_one_verts, check);

    check = {"P2", "A", "D", "B", "T2", "G", "J"};
    ASSERT_EQ(bipartite_two_verts, check);

    check = {"W1", "A", "B", "D", "K"};
    ASSERT_EQ(weighted_one_verts, check);
    check = {"W2", "A", "B", "D", "K"};
    ASSERT_EQ(weighted_two_verts, check);
}

TEST_F(GraphsTest, AsWeightedTest)
{
    auto simple_w = first_simple->AsWeighted(5);
    ASSERT_EQ(simple_w.ToString(), "WeightedGraph {A-B: 5, B-D: 5, E-D: 5, S1-A: 5, S1-B: 5}");

    auto complete_w = first_complete->AsWeighted(10);
    ASSERT_EQ(complete_w.ToString(),
              "WeightedGraph {A-B: 10, A-C1: 10, A-D: 10, A-F: 10, B-C1: 10, B-D: 10, B-F: 10, C1-D: 10, C1-F: 10, D-F: 10}");

    auto bip_w = first_bipartite->AsWeighted(1);
    ASSERT_EQ(bip_w.ToString(),
              "WeightedGraph {A-B: 1, A-G: 1, A-J: 1, A-T1: 1, D-B: 1, D-G: 1, D-J: 1, D-T1: 1, P1-B: 1, P1-G: 1, P1-J: 1, P1-T1: 1}");

    auto weight_w = first_weighted->AsWeighted(12);
    ASSERT_EQ(weight_w.ToString(), first_weighted->ToString());
}

TEST_F(GraphsTest, SimpleSum)
{
    auto sim_sim = *first_simple + *second_simple;
    ASSERT_EQ(sim_sim.ToString(), "SimpleGraph {S1-A, S1-B, A-B, B-D, E-D, S2-A, S2-D}");

    auto sim_com = *first_simple + *second_complete;
    ASSERT_EQ(sim_com.ToString(), "SimpleGraph {S1-A, S1-B, A-B, B-D, E-D, A-C2, A-D, A-F, B-C2, B-F, C2-D, C2-F, D-F}");

    auto sim_bip = *first_simple + *second_bipartite;
    ASSERT_EQ(sim_bip.ToString(),
              "SimpleGraph {S1-A, S1-B, A-B, B-D, E-D, A-G, A-J, A-T2, D-G, D-J, D-T2, P2-B, P2-G, P2-J, P2-T2}");

    ASSERT_THROW(*first_simple+*second_weighted, std::logic_error);
}

TEST_F(GraphsTest, CompleteSum)
{
    auto com_sim = *first_complete + *second_simple;
    ASSERT_EQ(com_sim.ToString(), "SimpleGraph {A-B, A-C1, A-D, A-F, B-C1, B-D, B-F, C1-D, C1-F, D-F, S2-A, S2-D, E-D}");

    auto com_com = *first_complete + *second_complete;
    ASSERT_EQ(com_com.ToString(), "CompleteGraph {A, B, C1, C2, D, F}");

    auto com_bip = *first_complete + *second_bipartite;
    ASSERT_EQ(com_bip.ToString(),
              "SimpleGraph {A-B, A-C1, A-D, A-F, B-C1, B-D, B-F, C1-D, C1-F, D-F, A-G, A-J, A-T2, D-G, D-J, D-T2, P2-B, P2-G, P2-J, P2-T2}");

    ASSERT_THROW(*first_complete+*second_weighted, std::logic_error);
}

TEST_F(GraphsTest, BipSum)
{
    auto bip_sim = *first_bipartite + *second_simple;
    ASSERT_EQ(bip_sim.ToString(), "SimpleGraph {A-B, A-G, A-J, A-T1, D-B, D-G, D-J, D-T1, P1-B, P1-G, P1-J, P1-T1, S2-A, S2-D, E-D}");

    auto bip_com = *first_bipartite + *second_complete;
    ASSERT_EQ(bip_com.ToString(), "SimpleGraph {A-B, A-G, A-J, A-T1, D-B, D-G, D-J, D-T1, P1-B, P1-G, P1-J, P1-T1, A-C2, A-D, A-F, B-C2, B-F, C2-D, C2-F, D-F}");

    auto bip_bip = *first_bipartite + *second_bipartite;
    ASSERT_EQ(bip_bip.ToString(),
              "BipartiteGraph {A, D, P1, P2}, {B, G, J, T1, T2}");

    ASSERT_THROW(*first_bipartite+*second_weighted, std::logic_error);
}

TEST_F(GraphsTest, WeightSum)
{
    ASSERT_THROW(*first_weighted+*second_simple, std::logic_error);
    ASSERT_THROW(*first_weighted+*second_complete, std::logic_error);
    ASSERT_THROW(*first_weighted+*second_bipartite, std::logic_error);

    auto sum_weighted = *first_weighted+*second_weighted;
    ASSERT_EQ(sum_weighted.ToString(),
              "WeightedGraph {A-B: 15, B-K: 10, K-D: 25, W1-A: 5, W1-K: 10, W2-A: 25, W2-D: 20}");
}

TEST_F(GraphsTest, SimpleMin)
{
    auto sim_sim = *first_simple - *second_simple;
    ASSERT_EQ(sim_sim.ToString(), "SimpleGraph {S1-A, S1-B}");

    auto sim_com = *first_simple - *second_complete;
    ASSERT_EQ(sim_com.ToString(), "SimpleGraph {S1-A, S1-B, E-D}");

    auto sim_bip = *first_simple - *second_bipartite;
    ASSERT_EQ(sim_bip.ToString(),
              "SimpleGraph {S1-A, S1-B, E-D}");

    auto sim_wet = *first_simple-*second_weighted;
    ASSERT_EQ(sim_wet.ToString(),
              "SimpleGraph {S1-A, S1-B, B-D, E-D}");
}


TEST_F(GraphsTest, CompleteMin)
{
    auto com_sim = *first_complete - *second_simple;
    ASSERT_EQ(com_sim.ToString(), "SimpleGraph {A-C1, A-D, A-F, B-C1, B-F, C1-D, C1-F, D-F}");

    auto com_com = *first_complete  - *second_complete;
    ASSERT_EQ(com_com.ToString(), "CompleteGraph {C1}");

    auto com_bip = *first_complete  - *second_bipartite;
    ASSERT_EQ(com_bip.ToString(),
              "SimpleGraph {A-C1, A-D, A-F, B-C1, B-F, C1-D, C1-F, D-F}");

    auto com_wet = *first_complete - *second_weighted;
    ASSERT_EQ(com_wet.ToString(),
              "SimpleGraph {A-C1, A-D, A-F, B-C1, B-D, B-F, C1-D, C1-F, D-F}");
}


TEST_F(GraphsTest, BipMin)
{
    auto bip_sim = *first_bipartite - *second_simple;
    ASSERT_EQ(bip_sim.ToString(), "SimpleGraph {A-G, A-J, A-T1, D-G, D-J, D-T1, P1-B, P1-G, P1-J, P1-T1}");

    auto bip_com = *first_bipartite - *second_complete;
    ASSERT_EQ(bip_com.ToString(), "SimpleGraph {A-G, A-J, A-T1, D-G, D-J, D-T1, P1-B, P1-G, P1-J, P1-T1}");

    auto bip_bip = *first_bipartite  - *second_bipartite;
    ASSERT_EQ(bip_bip.ToString(),
              "BipartiteGraph {P1}, {T1}");

    auto bip_wet = *first_bipartite - *second_weighted;
    ASSERT_EQ(bip_wet.ToString(),
              "SimpleGraph {A-G, A-J, A-T1, D-B, D-G, D-J, D-T1, P1-B, P1-G, P1-J, P1-T1}");
}


TEST_F(GraphsTest, WeightMin)
{
    auto wet_sim = *first_weighted - *second_simple;
    ASSERT_EQ(wet_sim.ToString(), "WeightedGraph {B-K: 20, K-D: 25, W1-A: 5, W1-K: 10}");

    auto wet_com = *first_weighted - *second_complete;
    ASSERT_EQ(wet_com.ToString(), "WeightedGraph {B-K: 20, K-D: 25, W1-A: 5, W1-K: 10}");

    auto wet_bip = *first_weighted  - *second_bipartite;
    ASSERT_EQ(wet_bip.ToString(),
              "WeightedGraph {B-K: 20, K-D: 25, W1-A: 5, W1-K: 10}");

    auto wet_wet = *first_weighted - *second_weighted;
    ASSERT_EQ(wet_wet.ToString(),
              "WeightedGraph {W1-A: 5, W1-K: 10}");
}

TEST_F(GraphsTest, PlusEqualOp)
{
    auto simple_graph = *first_simple;
    simple_graph += *second_complete;
    ASSERT_EQ(simple_graph.ToString(),
              "SimpleGraph {S1-A, S1-B, A-B, B-D, E-D, A-C2, A-D, A-F, B-C2, B-F, C2-D, C2-F, D-F}");
    auto complete_graph = *first_complete;
    ASSERT_THROW(complete_graph+=*second_simple, std::logic_error);
}

