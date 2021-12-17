#include <gtest/gtest.h>
#include "factory.h"

class graphCreation : public ::testing::Test {
protected:
    virtual void SetUp() {

        edg = {{{"A", "B"}}, {{"B", "C"}}, {{"C", "D"}}, {{"D", "E"}}, {{"E", "F"}}};
        verts = {"A", "B", "C", "D", "E", "F"};
        seconds = {"Q", "R", "T"};
        inters = {"Q", "R", "T", "C"};
        weits = {1, 5, 12, 77, 124};
        wrong_weits = {1, 5, 3};
        other_wrong = {10, 11, 12, 13, 14, 15, 16};

    }

    TFactory factory;
    edges edg;
    vertices verts, seconds, inters;
    weights weits, wrong_weits, other_wrong;
};

TEST_F(graphCreation, noExceptTest)
{
    ASSERT_NO_THROW(factory.CreateObject("simple", edg));
    ASSERT_NO_THROW(factory.CreateObject("complete", verts));
    ASSERT_NO_THROW(factory.CreateObject("bipartite", verts, seconds));
    ASSERT_NO_THROW(factory.CreateObject("weighted", edg, weits));
}


TEST_F(graphCreation, logicErrorsTest)
{
    ASSERT_THROW(factory.CreateObject("bipartite", verts, verts), std::logic_error);
    ASSERT_THROW(factory.CreateObject("bipartite", verts, inters), std::logic_error);
    ASSERT_THROW(factory.CreateObject("bipartite", inters, verts), std::logic_error);
    ASSERT_THROW(factory.CreateObject("weighted", edg, wrong_weits), std::logic_error);
    ASSERT_THROW(factory.CreateObject("weighted", edg, other_wrong), std::logic_error);
}

TEST_F(graphCreation, wrongParamsTest)
{

    ASSERT_THROW(factory.CreateObject("simple", verts), std::logic_error);
    ASSERT_THROW(factory.CreateObject("simple", verts, seconds), std::logic_error);
    ASSERT_THROW(factory.CreateObject("simple", edg, weits), std::logic_error);

    ASSERT_THROW(factory.CreateObject("complete", edg), std::logic_error);
    ASSERT_THROW(factory.CreateObject("complete", verts, seconds), std::logic_error);
    ASSERT_THROW(factory.CreateObject("complete", edg, weits), std::logic_error);

    ASSERT_THROW(factory.CreateObject("bipartite", verts), std::logic_error);
    ASSERT_THROW(factory.CreateObject("bipartite", edg), std::logic_error);
    ASSERT_THROW(factory.CreateObject("bipartite", edg, weits), std::logic_error);

    ASSERT_THROW(factory.CreateObject("weighted", verts), std::logic_error);
    ASSERT_THROW(factory.CreateObject("weighted", edg), std::logic_error);
    ASSERT_THROW(factory.CreateObject("weighted", verts, seconds), std::logic_error);
}