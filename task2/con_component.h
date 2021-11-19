#ifndef TASK2_CON_COMPONENT_H
#define TASK2_CON_COMPONENT_H

#include "types.h"

class con_component
{
    std::size_t number = 0;
    dependency jobs_dep;
    jobList all_jobs;
    list_job visited;

public:

    con_component() = default;
    con_component(std::size_t num, jobList jobnums) : number(num), all_jobs(std::move(jobnums))  {};
    con_component(std::size_t num, dependency all_jobs) : number(num), jobs_dep(std::move(all_jobs)) {};
    con_component(std::size_t num, dependency all_jobs, jobList jobnums) : number(num), jobs_dep(std::move(all_jobs)), all_jobs(std::move(jobnums)) {};

    [[maybe_unused]] [[nodiscard]] std::size_t get_num() const {return number;}
    jobList get_jobs() {return all_jobs;}
    dependency get_dependencies() {return jobs_dep;}
    std::shared_ptr<Job> get_job_by_num(std::size_t num);

    [[maybe_unused]] void print();
    void regulate_degrees();
    [[maybe_unused]] void all_dependence();
    void generate_depend();
    void fill_all_dependencies();
    static std::size_t get_list_sum(const list_job& degs);
    [[maybe_unused]] std::size_t get_cur_deg(const std::shared_ptr<Job>& job);
    static bool graph_condition(list_job degrees);
    static bool graph_condition(deg_pair check_map);
    bool degrees_correct(deg_pair deg_dict);
    void set_degs(const deg_pair& degs);
    static deg_pair transform_map(const deg_pair& degs, std::size_t border);

    [[maybe_unused]] bool find_cycles();
    bool rec_cycles(std::size_t num_to_find, std::size_t cur_num);

};
#endif //TASK2_CON_COMPONENT_H
