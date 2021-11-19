#ifndef TASK2_RUN_THREAD_H
#define TASK2_RUN_THREAD_H

#include <thread>
#include <atomic>
#include "threads.h"

class run_thread
{
    pthread_t id{};
    thread_schedule obj;
    std::size_t cycle_num = 20, iter_num = 5, divisor = 5;
    std::size_t pos_num = 0;
    std::shared_ptr<std::thread> cur_thr;
    //std::set<solution> config_done;
    //jobList all_jobs;
public:

    void initialize(const std::string& jobs_name, const std::string& proc_name);

    pthread_t* get_id_addr() {return &id;}
    [[nodiscard]] pthread_t get_id() const {return id;}
    [[nodiscard]] std::size_t get_cycle_num() const {return cycle_num;}
    [[nodiscard]] std::size_t get_iter_num() const {return iter_num;}
    [[nodiscard]] std::size_t get_div() const {return divisor;}
    [[nodiscard]] std::size_t get_num() const {return pos_num;}
    [[nodiscard]] auto get_thread() const {return cur_thr;}

    void set_thread(std::thread new_thr) {cur_thr = std::make_shared<std::thread>(std::move(new_thr));}
    auto get_thread() {return cur_thr;}

    std::size_t get_best_loss() {return obj.get_best_loss();}
    solution get_best() {return obj.get_best();}
    void set_loss(std::size_t num) {obj.set_loss(num);}
    void set_best(solution best_sol) {obj.set_best(std::move(best_sol));}
    void set_config(const solution& sol) {obj.set_config(sol);}
    void set_num(const std::size_t p_num) {pos_num = p_num;}
    solution get_solution() {return obj.get_best();}
    solution get_config() {return obj.get_config();}
    std::size_t compute() {return obj.compute();}
    solution transform_solution() {return obj.transform_solution();}
    double temp_change(double begin_temp, std::size_t num) {return obj.temp_change(begin_temp, num);}
    double get_temp() {return obj.get_temp();}

    void set_cycle_num(std::size_t cyc_num) {cycle_num=cyc_num;}
    void set_iter_num(std::size_t it_num) {iter_num=it_num;}
    void set_divisor(std::size_t div) {divisor=div;}

};

#endif //TASK2_RUN_THREAD_H
