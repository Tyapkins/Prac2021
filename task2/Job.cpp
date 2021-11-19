#include "Job.h"

bool Job::make_master(const std::shared_ptr<Job>& other_job)
{
    auto job_link = std::make_shared<Job>(*this);
    if (!other_job->make_dependent((this)))
        return false;
    degree++;
    return true;
}

bool Job::make_dependent(Job*  other_job)
{
    auto other_num = other_job->get_num();
    if (other_jobs.find(other_num) != other_jobs.end())
        return false;
    degree++;
    other_jobs.insert(other_num);
    return true;
}

bool Job::delete_master(const std::shared_ptr<Job>& ptr)
{
    auto search_num = ptr->get_num();
    if ((other_jobs.find(search_num)) == other_jobs.end())
        return false;
    other_jobs.erase(search_num);
    degree--;
    ptr->degree--;
    return true;
}

[[maybe_unused]] bool Job::delete_dependent(const std::shared_ptr<Job>& ptr_to_job)
{
    auto dep_job = std::make_shared<Job>(*this);
    return  ptr_to_job -> delete_master(dep_job);
}

std::size_t Job::get_rand_less(job_set job_nums)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distrib(*(job_nums.begin()), *(job_nums.rbegin()));
    auto num = distrib(gen);
    for(;
            ((job_nums.find(num) == job_nums.end()) ||
             (other_jobs.find(num) != other_jobs.end()) ||
             (num == number));
            num = distrib(gen));
    return num;
}

[[maybe_unused]] std::size_t Job::get_rand_big(job_set job_nums)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distrib(*(job_nums.begin()), *(job_nums.rbegin()));
    auto num = distrib(gen);
    for(;
            ((job_nums.find(num) == job_nums.end()) ||
             (other_jobs.find(num) == other_jobs.end()) ||
             (num == number));
            num = distrib(gen));
    return num;
}

