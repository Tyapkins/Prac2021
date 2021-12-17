#include <iostream>
#include <utility>
#include "factory.h"
#include "funcs.h"


class TFactory::TImpl {


    class ICreator {
    public:
        virtual ~ICreator() = default;
        [[nodiscard]] virtual std::shared_ptr<Graph> Create(container pars) const = 0;
    };

    using TCreatorPtr = std::shared_ptr<ICreator>;
    using TRegisteredCreators = std::map<std::string, TCreatorPtr>;
    TRegisteredCreators RegisteredCreators;

public:

    template<class TCurrentObject>
    class TCreator : public ICreator {
        [[nodiscard]] std::shared_ptr<Graph> Create(container pars) const override {
            return std::make_shared<TCurrentObject>(pars);
        }
    };


    TImpl() { RegisterAll(); }

    template<typename T>
    void RegisterCreator(const std::string &type) {
        RegisteredCreators[type] = std::make_shared<TCreator<T>>();
    }

    void RegisterAll() {
        RegisterCreator<BipartiteGraph>("bipartite");
        RegisterCreator<CompleteGraph>("complete");
        RegisterCreator<SimpleGraph>("simple");
        RegisterCreator<WeightedGraph>("weighted");
    }

    [[nodiscard]] std::shared_ptr<Graph> CreateObject(const std::string &n, container true_par) const {
        auto creator = RegisteredCreators.find(n);
        if (creator == RegisteredCreators.end()) {
            return nullptr;
        }
        else {
            return creator->second->Create(std::move(true_par));
        }
    }

    [[nodiscard]] std::vector<std::string> GetAvailableObjects() const {
        std::vector<std::string> result;
        for (const auto &creatorPair : RegisteredCreators) {
            result.push_back(creatorPair.first);
        }
        return result;
    }
};

void TFactory::PredCompile() const {
    CreateObject<edges>("", {});
    CreateObject<vertices>("", {});
    CreateObject<vertices, vertices>("", {}, {});
    CreateObject<edges, weights>("", {}, {});
}

TFactory::TFactory() : Impl(std::make_shared<TFactory::TImpl>()) {
    PredCompile();
};

TFactory::~TFactory() = default;

std::vector <std::string> TFactory::GetAvailableObjects() const {
    return Impl->GetAvailableObjects();
}

template<typename... Args>
std::shared_ptr<Graph> TFactory::CreateObject(const std::string &type, Args... args) const {
    container all_args;
    mas_to_container(all_args, args...);
    return Impl->CreateObject(type, all_args);
}

