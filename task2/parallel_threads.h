#ifndef TASK2_PARALLEL_THREADS_H
#define TASK2_PARALLEL_THREADS_H

#include "run_thread.h"
#include <chrono>
#include <fstream>

class all_threads
{
    std::size_t thread_num = 1;
    std::vector<std::shared_ptr<run_thread>> par_threads;

    std::map<int, std::atomic<bool>> job_pause, job_finished;

    std::set<solution> all_solutions;
    std::size_t best_lose = INTMAX_MAX;
    std::string jobs_name = "jobs_100_new_test.txt";
    std::string proc_name = "5_procs_100.txt";
    solution best_solut;

    improved_solution best_impr;

    std::atomic<bool> generated = true, transforming = false, exchanging = false;

public:

    //void set_best(solution new_best) {best_solut = std::move(new_best);}
    loss_and_sol get_best() {return std::make_pair(best_solut, best_lose);}

    void set_thread_num(std::size_t num) {thread_num = num;}
    void set_jobs_name(const std::string& name) {jobs_name = name;}
    void set_proc_name(const std::string& name) {proc_name = name;}

    std::shared_ptr<run_thread> get_thread_by_num(std::size_t num);
    std::set<solution> get_all_solution() {return all_solutions;}
    void add_to_solutions(const solution& sol) {all_solutions.insert(sol);}

    solution hard_change(std::shared_ptr<run_thread> thread);
    solution easy_change(std::shared_ptr<run_thread> thread);

    auto get_all_threads() {return par_threads;}

    loss_and_sol exchange_solutions();
    bool is_generated() {return generated;}
    bool is_transforming() {return transforming;}
    bool is_exchanging() {return exchanging;}

    void begin_transform() {transforming=true;}
    void end_transform() {transforming=false;}

    void begin_exchange() {exchanging=true;}
    void end_exchange() {exchanging=false;}

    void pause_job(std::size_t num) {job_pause[num] = true;}
    void unpause_job(std::size_t num) {job_pause[num] = false;}
    bool is_paused(std::size_t num) {return job_pause[num];}

    void finish_job(std::size_t num) {job_finished[num] = true;}

   // bool all_iterated() {return std::for_each(job_pause.begin(), job_pause.end(), [](std::pair<int, bool> par) {return  par.second;});}

    bool all_finished();
    bool all_iterated();

    void fill_data();
    double create_threads();

    void check_coherent(interval node_nums, interval proc_nums, num iter_num = 20,  num node_diff = 100, num proc_diff=1);
    void check_parallel(interval node_num, const list_job& thread_nums, num iter_num = 20, num node_diff = 100, num proc_num=5);

    explicit all_threads(std::size_t num) : thread_num(num) {};

    static solution cycle_with_saving(all_threads* true_obj, std::size_t obj_num);


};


#endif //TASK2_PARALLEL_THREADS_H
