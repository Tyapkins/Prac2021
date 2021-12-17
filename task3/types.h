#ifndef TASK3_TYPES_H
#define TASK3_TYPES_H

using name = std::string;
using vertices = std::set<name>;

struct edge {
    std::pair<name, name> val;
    explicit operator std::string() const {return val.first + "-" + val.second;}
    bool operator ==(const edge& other) const;
    bool operator !=(const edge& other) const {return (!(operator==(other)));}
    bool operator <(const edge& other) const;
};

using edges = std::vector<edge>;
using weight = unsigned long;
using weights = std::vector<weight>;
using container = std::vector<std::any>;
using check_type = const std::type_info*;
using type_vector = std::vector<check_type>;

#endif //TASK3_TYPES_H
