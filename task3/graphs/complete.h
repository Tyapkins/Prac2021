#ifndef TASK3_COMPLETE_H
#define TASK3_COMPLETE_H

#include "graph.h"
#include "simple.h"


class CompleteGraph : public virtual Graph, public SimpleGraph {

public:

    explicit CompleteGraph(const container& args);
    const std::string ToString() override;
    edges GetEdgesByVerts();

    CompleteGraph operator +(CompleteGraph second);

    CompleteGraph operator -(CompleteGraph second);
};


#endif //TASK3_COMPLETE_H
