#ifndef TASK2_PARSING_H
#define TASK2_PARSING_H

#include "Job.h"
#include "Processor.h"

std::string no_comment(const std::string& comment);
std::string only_one_space(std::string many_spaces);
std::string remove_last_space(std::string last_space);
std::string clean_string(const std::string& not_cleaned);
list_job extract_num(std::string num_string);
jobList read_jobs(const std::string& filename);
procList read_procs(const std::string& filename);
void jobs_to_file(const jobList& jobs, const std::string& filename = "gen_jobs.txt");
void procs_to_file(const procList& procs, const std::string& filename = "processors.txt");
std::string gen_job_filename(std::size_t node_num, std::size_t comp_num=2, std::size_t deg_mean=20);
std::string gen_proc_filename(std::size_t proc_num, std::size_t node_num);

#endif //TASK2_PARSING_H
