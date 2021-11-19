#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include "parsing.h"

std::string no_comment(const std::string& comment){
    return comment.substr(0, (comment.find('#')));
}

std::string only_one_space(std::string many_spaces)
{
    std::string one_space = std::move(many_spaces);

    auto two_spaces = [](unsigned char const a)
    {
        static bool pred_space = false;
        bool cur_space = std::isspace(a);
        auto ret_value = cur_space && pred_space;
        pred_space = cur_space;
        return ret_value;
    };

    one_space.erase(std::remove_if(one_space.begin(),
                                   one_space.end(),
                                     two_spaces), one_space.end());

    return one_space;
}

std::string remove_last_space(std::string last_space)
{
    std::string no_space = std::move(last_space);
    no_space.erase(std::remove_if(std::prev(no_space.end()),
                                  no_space.end(),
                                                    [](unsigned char const a) {return std::isspace(a);}),
                   no_space.end());
    return no_space;
}

std::string clean_string(const std::string& not_cleaned)
{
    return remove_last_space(only_one_space(no_comment(not_cleaned)));
}

list_job extract_num(std::string num_string)
{
    list_job nums;
    std::string extract_from = std::move(num_string);
    std::size_t pos;
    while ( (pos = extract_from.find_first_not_of("0123456789")) != std::string::npos)
    {
        nums.push_back(std::stoul(extract_from.substr(0,pos)));
        extract_from.erase(0, pos+1);
    }
    nums.push_back(std::stoul(extract_from));
    return nums;
}

jobList read_jobs(const std::string& filename)
{
    std::string buffer;
    std::ifstream jobs(filename);
    std::size_t job_num = 0, cur_num = 0;
    jobList true_jobs;
    while (std::getline(jobs, buffer)) {
        if (!(buffer.empty()))
        {
            auto cleaned_string = clean_string(buffer);
            if (!(cleaned_string.empty())) {
                if (job_num == 0) {
                    job_num = std::stoul(cleaned_string);
                    if (job_num <= 0)
                        std::cout << "YOU'RE REALLY WRONG" << std::endl;
                }
                else {
                    auto nums = extract_num(cleaned_string);
                    dur_time dur = nums.at(0);
                    nums.erase(nums.begin());
                    job_set depend_set;
                    depend_set.insert(nums.begin(), nums.end());
                    Job new_job(cur_num++, dur, depend_set);
                    auto job_ptr = std::make_shared<Job>(new_job);
                    //true_jobs.emplace_back(cur_num++, dur, depend_set);
                    true_jobs.push_back(job_ptr);
                }
            }
        }
    }
    return true_jobs;
}

procList read_procs(const std::string& filename)
{
    std::string buffer;
    std::ifstream jobs(filename);
    std::size_t proc_num = -1;
    procList procs;
    list_job used_nums;
    while (std::getline(jobs, buffer)) {
        if (!(buffer.empty()))
        {
            auto cleaned_string = clean_string(buffer);
            if (!(cleaned_string.empty())) {
                if (proc_num == -1) {
                    proc_num = std::stoul(cleaned_string);
                }
                else {
                    auto after_proc_num = cleaned_string.find(':');
                    auto cur_num = std::stoul(cleaned_string.substr(0, after_proc_num));
                    used_nums.insert(used_nums.begin(), cur_num);
                    cleaned_string.erase(0, after_proc_num+2);
                    auto nums = extract_num(cleaned_string);
                    jobDeque job_deq;
                    job_deq.insert(job_deq.begin(), nums.begin(), nums.end());
                    //job_set set_nums;
                    //set_nums.insert(nums.begin(), nums.end());
                    Processor new_proc(cur_num, job_deq);
                    procs.push_back(std::make_shared<Processor>(new_proc));
                }
            }
        }
    }
    for (int i = 0; i < proc_num; i++)
    {
        if (std::find(used_nums.begin(), used_nums.end(), i) == used_nums.end()) {
            Processor new_proc(i);
            procs.push_back(std::make_shared<Processor>(new_proc));
        }
    }
    return procs;
}

void jobs_to_file(const jobList& jobs, const std::string& filename)
{
    std::ofstream dot_file;
    dot_file.open(filename, std::ios::out | std::ios::trunc);
    dot_file << jobs.size() << std::endl << std::endl;
    std::size_t max_num = 0, max_dur = 0, max_dep = 0, max_job_len = 0;
    for (const auto& job : jobs)
    {
        max_num = std::max(max_num, job->get_num());
        max_dur = std::max(max_dur, job->get_durat());
        auto cur_dep = job->get_depend();
        //std::cout << cur_dep.size() << std::endl;
        max_dep = std::max(max_dep, cur_dep.size());
        for (auto job_num : cur_dep)
            max_job_len = std::max(max_job_len, job_num);
    }
    //max_num = std::to_string(max_num).size();
    max_dur = std::to_string(max_dur).size();
    max_job_len = std::to_string(max_job_len).size();
    //std::cout << max_num << " " << max_dur << " " << max_dep << " " << max_job_len << std::endl;
    auto space_num = max_dur + (max_job_len+1)*(max_dep-1) + 8;
    //std::cout << space_num << std::endl;
    for (const auto& job : jobs) {
        auto dur = job->get_durat();
        auto cur_dur = std::to_string(dur).size();
        dot_file << dur;
        auto depend_num = job->get_depend();
        //std::cout << "cur_dur = " << cur_dur << std::endl;
        if (depend_num.empty()) {
            //std::cout << "Tab = " << space_num - cur_dur + 8 << std::endl;
            dot_file << std::setw(space_num - cur_dur + 8);
        } else {
            //std::cout << "Job Tab = " << max_dur - cur_dur + 8 << std::endl;
            dot_file << std::setw(max_dur - cur_dur + 8);
            for (auto dep_num : depend_num) {
                dot_file << dep_num << std::setw(max_job_len+1);
            }
            //std::cout << (max_dep-depend_num.size())*(max_job_len+1) << std::endl;
            dot_file << std::setw(((max_dep)-depend_num.size())*(max_job_len+1)+8);
        }
        dot_file << "# job " << job->get_num() << std::endl << std::endl;
    }
}

void procs_to_file(const procList& procs, const std::string& filename)
{
    std::ofstream dot_file;
    dot_file.open(filename, std::ios::out | std::ios::trunc);
    dot_file << procs.size() << std::endl << std::endl;
    std::size_t max_num = 0, max_size = 0, max_job_size = 0;
    for (const auto& proc : procs) {
        max_num = std::max(max_num, proc->get_num());
        auto all_jobs = proc->get_job_nums();
        max_size = std::max(max_size, all_jobs.size());
        for (auto job_num : all_jobs)
            max_job_size = std::max(max_job_size, job_num);
    }
    max_num = std::to_string(max_num).size();
    max_job_size = std::to_string(max_job_size).size();
    for (const auto& proc : procs) {
        auto cur_num = proc->get_num();
        auto cur_jobs = proc->get_job_nums();
        if (!cur_jobs.empty())
        {
            dot_file << "# proc number " << proc->get_num() << std::endl << std::endl;
            dot_file << cur_num << std::setw(max_num - std::to_string(cur_num).size() + 1) << ":" << std::setw(max_job_size+2);
            for (auto job : cur_jobs)
                dot_file << job << std::setw(max_job_size+2);
        }
        else
            dot_file << "# There are no jobs for processor " << proc->get_num();
        dot_file << std::endl << std::endl;
    }
}

std::string gen_job_filename(std::size_t node_num, std::size_t comp_num, std::size_t deg_mean)
{
    return JOB_PATH+"jobs_" + std::to_string(node_num) + "_"
           + std::to_string(comp_num) + "_" + std::to_string(deg_mean);
}

std::string gen_proc_filename(std::size_t proc_num, std::size_t node_num)
{
    return PROC_PATH+std::to_string(proc_num)+"_procs_"+std::to_string(node_num)+".txt";
}