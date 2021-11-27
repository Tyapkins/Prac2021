#ifndef TASK2_THREADS_H
#define TASK2_THREADS_H

#include "graph.h"

class thread_schedule
{
    jobList all_jobs{};
    conditions cur_all_dep;
    job_set visited;
    procList procs{};
    deg_pair job_procs{};
    done_list jobs_done;
    std::size_t best_loss = INTMAX_MAX;
    solution best_sol;
    improved_solution improved_best;

    bool tried_all = false;

    double Temp = 1000;

    bool is_job_done(std::size_t num) {return jobs_done[num];}
    bool all_jobs_are_done();

public:

    std::size_t compute();

    bool operation_1(std::size_t job_num, std::size_t new_proc_num);
    bool operation_2(std::size_t job_num);

    bool is_all_tried() {return tried_all;}

    solution get_jobs_to_procs();
    job_set get_chain_dep(std::size_t num);
    job_set find_slaves(std::size_t num);
    job_set get_all_deps(std::size_t num);

    void fill_jobs_proc();

    solution get_config();
    [[nodiscard]] std::size_t get_best_loss() const {return best_loss;}

    solution transform_solution();
    void set_config(const solution& config);
    void set_loss(std::size_t num) {best_loss = num;}

    solution get_best() {return best_sol;}
    void set_best(solution sol) {best_sol = std::move(sol);}

    improved_solution get_improved() {return improved_best;}
    void set_improved(improved_solution impr_sol) {improved_best = std::move(impr_sol);}
    improved_solution get_improved_config();

    std::shared_ptr<Job> get_job_by_num(std::size_t num);
    bool can_be_done(std::size_t num);
    solution iterate(std::size_t iteration_num);

    solution full_cycle(std::size_t cycle_num, std::size_t iteration_num);


    [[nodiscard]] double get_temp() const {return Temp;}
    double temp_change(double tempr, double iter_num);

    void create_procs(std::size_t proc_num);

    void set_procs(procList nums) {procs=std::move(nums);}
    void set_jobs(jobList jobs) {all_jobs=std::move(jobs);}

    static jobList generate_job_file(std::size_t node_num,
            interval weight_int,
            std::size_t weight_disp,
            parameters components,
            parameters degrees,
    const std::string& filename);

    procList generate_proc_file(std::size_t proc_num,
                        std::size_t node_num,
                        const std::string& filename);

    void generate_files(std::size_t proc_num,
                        std::size_t node_num,
                        interval weight_int, std::size_t weight_disp,
                        parameters components,
                        parameters degrees,
                        const std::string& filename);

    void gen_file_interval(interval proc_interval, interval node_interval,
                           num proc_diff = 1, num node_diff = 100,
                           num comp_num = 2, num deg_num = 20);


    solution do_anneal(std::size_t proc_num,
                   std::size_t node_num,
                   std::size_t cycle_num,
                   std::size_t iteration_num,
                   interval weight_int, std::size_t weight_disp,
                   parameters components,
                   parameters degrees,
                   const std::string& filename);




    thread_schedule() = default;


    std::size_t get_floor_limit(std::size_t proc_num, std::size_t job_num);
    std::size_t get_upper_limit(std::size_t proc_num, std::size_t job_num);
    std::shared_ptr<Processor> get_proc_by_num(std::size_t proc_num);
    std::size_t get_proc_by_job_num(std::size_t job_num) {return job_procs[job_num];}

    [[maybe_unused]] explicit thread_schedule(procList processors) : procs(std::move(processors)) {};

    [[maybe_unused]] explicit thread_schedule(jobList jobs_to_do) : all_jobs(std::move(jobs_to_do)) {};

    [[maybe_unused]] thread_schedule (jobList jobs_to_do, procList processors) : procs(std::move(processors)), all_jobs(std::move(jobs_to_do)) {};


};

#endif //TASK2_THREADS_H
