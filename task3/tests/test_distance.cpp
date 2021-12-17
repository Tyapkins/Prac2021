#include <gtest/gtest.h>
#include "factory.h"
#include "distance.h"

class distanceChecking : public ::testing::Test {
protected:
    virtual void SetUp() {

        TFactory new_fac;

        edges weight_edg = {{{"A", "B"}}, {{"A", "C"}}, {{"A", "F"}}, {{"B", "C"}}, {{"B", "D"}},
                            {{"C", "D"}}, {{"C", "F"}}, {{"D", "E"}}, {{"E", "F"}}};
        weights con_weights = {7, 9, 14, 10, 15, 11, 2, 6, 9};
        con_graph = *(std::dynamic_pointer_cast<WeightedGraph>(new_fac.CreateObject("weighted", weight_edg, con_weights)));

        edges long_edg = {{{"A", "B"}}, {{"B", "C"}}, {{"C", "D"}}, {{"D", "E"}}, {{"E", "F"}}};
        weights long_weights = {5, 10, 3, 2, 4};
        con_long_graph = *(std::dynamic_pointer_cast<WeightedGraph>(new_fac.CreateObject("weighted", long_edg, long_weights)));

        edges non_con_edg = {{{"A", "B"}}, {{"B", "C"}}, {{"C", "D"}}, {{"E", "F"}}, {{"F", "G"}}};
        weights non_con_weights = {10, 11, 12, 13, 14};
        noncon_graph = *(std::dynamic_pointer_cast<WeightedGraph>(new_fac.CreateObject("weighted", non_con_edg, non_con_weights)));

    }
    WeightedGraph con_graph, con_long_graph, noncon_graph;
};


TEST_F(distanceChecking, connected_checking)
{
    ASSERT_EQ(is_connected(con_graph), true);
    ASSERT_EQ(is_connected(con_long_graph), true);
    ASSERT_EQ(is_connected(noncon_graph), false);
}

TEST_F(distanceChecking, distanceCheck)
{
    edges path = {{{"A", "C"}}, {{"C", "F"}}, {{"F", "E"}}};
    ASSERT_EQ(shortest_way(con_graph, "A", "E"), path);
    path = {{{"A", "C"}}, {{"C", "F"}}};
    ASSERT_EQ(shortest_way(con_graph, "A", "F"), path);
    path = {{{"B", "D"}}, {{"D", "E"}}};
    ASSERT_EQ(shortest_way(con_graph, "B", "E"), path);
    path = {{{"A", "B"}}, {{"B", "C"}}, {{"C", "D"}}};
    ASSERT_EQ(shortest_way(con_long_graph, "A", "D"), path);
    ASSERT_THROW(shortest_way(noncon_graph, "A", "F"), std::logic_error);
}