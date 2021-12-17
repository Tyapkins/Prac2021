#ifndef TASK3_FUNCS_H
#define TASK3_FUNCS_H

#include <string>
#include "graphs/graph.h"

template<typename T>
std::string Graph::VectorToString(T vec) {
    std::string final = "{";
    auto iter = vec.begin();
    bool not_first = false;
    while (iter != vec.end()) {
        if (not_first)
            final += ", ";
        else
            not_first = true;
        final += std::string(*iter);
        iter++;
    }
    final += "}";
    return final;
}

template <typename ForwardIterator>
ForwardIterator remove_duplicates( ForwardIterator first, ForwardIterator last )
{
    auto new_last = first;
    for (auto current = first; current != last; ++current )
    {
        if (std::find( first, new_last, *current ) == new_last)
        {
            if (new_last != current)
                *new_last = *current;
            ++new_last;
        }
    }
    return new_last;
}

template <typename... Args>
container mas_to_container(container mas, Args... args) {
    return mas;
};

template <typename First, typename... Args>
container mas_to_container(container& mas, First next_arg, Args... args)
{
    mas.template emplace_back(next_arg);
    return mas_to_container(mas, args...);
}

template <typename T, typename K>
std::pair<std::vector<T>,std::vector<K>> map_to_vectors(std::map<T,K> cur_map)
{
    std::vector<T> keys;
    std::vector<K> values;
    keys.reserve(cur_map.size());
    values.reserve(cur_map.size());
    for (const auto& keyValue : cur_map)
    {
        keys.push_back(keyValue.first);
        values.push_back(keyValue.second);
    }
    return {keys, values};
}

#endif //TASK3_FUNCS_H
