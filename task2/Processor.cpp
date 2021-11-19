#include <algorithm>
#include "Processor.h"

[[maybe_unused]] void Processor::next_job()
{
    std::rotate(job_nums.begin(), job_nums.begin()+1, job_nums.end());
}

void Processor::remove_job(std::size_t job_num)
{

    auto job_pos = std::find(job_nums.begin(), job_nums.end(), job_num);
    if (job_pos != job_nums.end())
        job_nums.erase(job_pos);
}

void Processor::add_job(std::size_t job_num, std::size_t pos)
{
    if ((pos < job_nums.size()) && (pos > 0))
        job_nums.insert(job_nums.begin()+pos, job_num);
    else if (pos > 0)
        job_nums.push_back(job_num);
    else
        job_nums.push_front(job_num);
}

