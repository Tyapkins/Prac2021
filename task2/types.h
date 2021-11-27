#ifndef TASK2_TYPES_H
#define TASK2_TYPES_H

#include <memory>
#include <utility>
#include <deque>
#include <vector>
#include <set>
#include <map>
#include <deque>
#include <random>
#include <iostream>
#include <algorithm>

typedef unsigned long num;
typedef size_t dur_time;
typedef std::vector<num> list_job;
typedef std::deque<num> jobDeque;
typedef std::set<num> job_set;
typedef std::pair<num, num> interval;
typedef std::pair<double, double> parameters;
typedef std::map<std::size_t, std::size_t> deg_pair;

class Job;
class Processor;
class con_component;

typedef std::vector<std::shared_ptr<Job>> jobList;
typedef std::vector<std::shared_ptr<Processor>> procList;
typedef std::vector<con_component> full_graph;
[[maybe_unused]] typedef std::map<num, list_job> dependency;

typedef std::map<std::size_t, bool> done_list;
typedef std::map<std::size_t, job_set> conditions;
typedef std::set<jobDeque> solution;
typedef std::pair<solution, std::size_t> loss_and_sol;

typedef std::pair<jobDeque, num> proc_and_end;
typedef std::set<proc_and_end> improved_solution;


const std::string PROC_PATH = "procs/";
const std::string JOB_PATH = "jobs/";
const std::string GRAPH_PATH = "graphs/";

#endif //TASK2_TYPES_H
