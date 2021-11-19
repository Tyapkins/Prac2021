#include "types.h"

#ifndef TASK2_JOB_H
#define TASK2_JOB_H

class Job
{
    std::size_t number = 0, degree = 0;
    dur_time duration = 0;
    job_set other_jobs;

public:

    Job(dur_time num, dur_time durat, job_set jobs): number(num), duration(durat), other_jobs(std::move(jobs)) {};
    Job(dur_time num, dur_time durat, std::size_t deg): number(num), duration(durat), degree(deg) {};

    [[nodiscard]] std::size_t get_num() const {return number;}
    [[nodiscard]] std::size_t get_deg() const {return degree;}
    [[nodiscard]] dur_time get_durat() const {return duration;}
    job_set get_depend() {return other_jobs;}

    [[maybe_unused]] void set_num(std::size_t num) {number=num;}
    void set_deg(std::size_t deg) {degree=deg;}

    [[maybe_unused]] void clear_depend() {other_jobs.clear();}


    bool make_master(const std::shared_ptr<Job>& other_job);
    bool make_dependent(Job* other_job);
    bool delete_master(const std::shared_ptr<Job>(&ptr));

    [[maybe_unused]] bool delete_dependent(const std::shared_ptr<Job>&  ptr);

    std::size_t get_rand_less(job_set job_nums);

    [[maybe_unused]] std::size_t get_rand_big(job_set job_nums);
};

#endif //TASK2_JOB_H
