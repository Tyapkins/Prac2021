#ifndef TASK3_FACTORY_H
#define TASK3_FACTORY_H

#include <memory>
#include <algorithm>
#include "graphs.h"

class TFactory {
    class TImpl;
    std::shared_ptr<const TImpl> Impl;

    void PredCompile() const;

public:
    TFactory();
    ~TFactory();

    template <typename... Args>
    std::shared_ptr<Graph> CreateObject(const std::string &n, Args... args) const;

    [[nodiscard]] std::vector<name> GetAvailableObjects() const;
};

#endif
