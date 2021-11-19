#include "types.h"

#ifndef TASK2_PROCESSOR_H
#define TASK2_PROCESSOR_H


class Processor
{
    std::size_t number = 0;
    jobDeque job_nums;

public:

    Processor(std::size_t num, jobDeque jobs) : number(num), job_nums(std::move(jobs)) {};
    explicit Processor(std::size_t num) : number(num) {};
    [[nodiscard]] std::size_t get_num() const {return number;}
    jobDeque get_job_nums() {return job_nums;}

    [[maybe_unused]] void next_job();

    void remove_job(std::size_t job_num);
    void add_job(std::size_t job_num, std::size_t pos);

    void set_job_order(const jobDeque& order) {
        clear_jobs();
        for (auto num : order)
            job_nums.push_back(num);
    };
    void clear_jobs() {job_nums.clear();}
};

#endif //TASK2_PROCESSOR_H
