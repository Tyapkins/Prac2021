#include "con_component.h"
#include "types.h"
#include "Job.h"


template<typename T1, typename T2>
std::multimap<T2, T1> reverse_map(std::map<T1, T2> some_map)
{
    std::multimap<T2, T1> reversed_map;
    for (auto el: some_map)
        reversed_map.insert(std::make_pair(el.second, el.first));
    return reversed_map;
}


[[maybe_unused]] void con_component::print()
{

    for (const auto& job : all_jobs)
    {
        auto check = job->get_depend();
        std::cout << "Job " << job->get_num() << ": " << job->get_durat() << " | " << job->get_deg() << std::endl;
        std::cout << "Dependencies: ";
        for (auto num : check)
            std::cout << num << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl << std::endl;
}

[[maybe_unused]] void con_component::all_dependence()
{

    for (const auto& job : all_jobs)
    {
        for (const auto& other_job : all_jobs)
        {
            if (job != other_job)
                job -> make_master(other_job);
        }
    }
}

std::shared_ptr<Job> con_component::get_job_by_num(std::size_t num)
{
    for (auto job : all_jobs)
        if (job->get_num() == num)
            return job;
    std::cout << "WHAT" << std::endl;
    return nullptr;
}

std::size_t con_component::get_list_sum(const list_job& degs)
{
    std::size_t deg_sum = 0;
    for (auto deg: degs)
        deg_sum += deg;
    return deg_sum;
}

deg_pair con_component::transform_map(const deg_pair& degs, std::size_t border)
{
    deg_pair new_degs;
    list_job only_degs;
    std::size_t min_num = 0;
    for (auto deg : degs)
        new_degs.insert(deg);
    for (auto & new_deg : new_degs)
    {
        if (new_deg.second >= border) {
            new_deg.second = border;
            min_num++;
        }
    }
    min_num = (min_num < border) ? min_num : border;
    for (auto & new_deg : new_degs)
        if (new_deg.second < min_num)
            new_deg.second = min_num;
    for (auto & new_deg : new_degs)
        only_degs.push_back(new_deg.second);
    if (get_list_sum(only_degs)%2 == 1)
    {
        auto sorted_deg = reverse_map(new_degs);
        for (auto & new_deg : sorted_deg)
        {
            if (new_deg.first < border) {
                new_degs[new_deg.second] += 1;
                break;
            }
        }
    }
    return new_degs;
}

void con_component::set_degs(const deg_pair& degs)
{
    for (auto el : degs)
        get_job_by_num(el.first) -> set_deg(el.second);
}



void con_component::regulate_degrees()
{
    deg_pair degrees;

    for (const auto& job : all_jobs) {
        degrees.insert(std::make_pair(job->get_num(), job->get_deg()));
    }

    auto true_degs = transform_map(degrees, all_jobs.size()-1);

    while (!graph_condition(true_degs)) {
        auto rev_map = reverse_map(true_degs);
        auto biggest = rev_map.begin();
        true_degs[biggest->second] += 1;
        true_degs[std::next(biggest)->second] += 1;
        true_degs = transform_map(true_degs, all_jobs.size()-1);
    }
    set_degs(true_degs);
}

[[maybe_unused]] std::size_t con_component::get_cur_deg(const std::shared_ptr<Job>& job)
{
    std::size_t num = job->get_depend().size();
    for (const auto& other_job : all_jobs)
    {
        if (other_job != job) {
            auto other_set = other_job->get_depend();
            if (other_set.find(job->get_num()) != other_set.end())
                num += 1;
        }
    }
    return num;
}

bool con_component::graph_condition(std::map<std::size_t, std::size_t> num_map)
{
    list_job deg_list;
    auto rev_map = reverse_map(std::move(num_map));
    for (auto num = rev_map.rbegin(); num != rev_map.rend(); num++) {
        deg_list.push_back(num->first);
    }
    return graph_condition(deg_list);
}

bool con_component::graph_condition(list_job degrees)
{
    std::size_t node_num = degrees.size();
    for (int k = 0; k < node_num; k++)
    {
        std::size_t sum_degrees = 0;
        for (int i = 0; i <= k; i++)
            sum_degrees += degrees[i];
        std::size_t var = k*(k+1);
        for (int i = k+1; i < node_num; i++)
            var += degrees[i] < (k+1) ? degrees[i] : (k+1);
        if (sum_degrees > var)
            return false;
    }
    return true;
}

void con_component::fill_all_dependencies()
{
    jobs_dep.clear();
    job_set found_job;
    for (const auto& new_job : all_jobs)
    {
        for (auto master_job : new_job->get_depend())
        {
            if (jobs_dep.find(master_job) == jobs_dep.end()) {
                list_job new_list = {new_job->get_num()};
                jobs_dep.insert(std::make_pair(master_job, new_list));
                found_job.insert(master_job);
                found_job.insert(new_job->get_num());
            }
            else {
                jobs_dep[master_job].push_back(new_job->get_num());
                found_job.insert(new_job->get_num());
            }
        }
    }
    list_job empty_list;
    for (const auto& new_job : all_jobs)
    {
        if (found_job.find(new_job->get_num()) == found_job.end())
            jobs_dep.insert(std::make_pair(new_job->get_num(), empty_list));
    }
}

bool con_component::degrees_correct(std::map<std::size_t, std::size_t> deg_dict)
{
    for (const auto& job : all_jobs)
        if (job->get_deg() != deg_dict[job->get_num()])
            return false;
    return true;
}

void con_component::generate_depend()
{
    job_set job_nums;
    deg_pair degrees_ideal;
    list_job ord_jobs;

    for (const auto& job : all_jobs) {
        job_nums.insert(job->get_num());
        degrees_ideal[job->get_num()] = job->get_deg();
        job->set_deg(0);
    }

    auto degrees_ord = reverse_map(degrees_ideal);

    list_job degs;

    for (auto it = degrees_ord.rbegin(); it != degrees_ord.rend(); it++) {
        ord_jobs.push_back(it->second);
        degs.push_back(it->first);
    }

    while (!(degrees_correct(degrees_ideal)))
    {
        auto cur_num = *(ord_jobs.begin());
        auto cur_job = get_job_by_num(cur_num);
        auto need_deg = degrees_ideal[cur_num];
        auto cur_dep = cur_job->get_depend();

        job_set added_set;
        while (cur_job->get_deg() != need_deg)
        {
            std::size_t ind = 0;
            auto new_num = ord_jobs[ind];
            while (
                    (
                            (job_nums.find(new_num) == job_nums.end()) ||
                            (new_num == cur_num) ||
                            (added_set.find(new_num) != added_set.end())
                    )
                    ) {
                new_num = ord_jobs[ind++];
            }
            while (added_set.find(new_num) != added_set.end())
                new_num = cur_job->get_rand_less(job_nums);

            auto new_job = get_job_by_num(new_num);
            cur_job->make_master(new_job);
            added_set.insert(new_num);
            if (new_job->get_deg() == degrees_ideal[new_num]) {
                job_nums.erase(new_num);
                ord_jobs.erase(std::find(ord_jobs.begin(), ord_jobs.end(), new_num));
            }
        }
        job_nums.erase(cur_num);
        degrees_ord.clear();
        for (const auto& job : all_jobs)
        {
            auto new_num = job->get_num();
            degrees_ord.insert(std::make_pair(degrees_ideal[new_num] - job->get_deg(), new_num));
        }
        ord_jobs.clear();
        for (auto it = degrees_ord.rbegin(); it != degrees_ord.rend(); it++) {
            ord_jobs.push_back(it->second);
        }
    }
}

bool con_component::rec_cycles(std::size_t num_to_find, std::size_t cur_num)
{
    visited.push_back(cur_num);
    auto list_to_find = jobs_dep[cur_num];
    if (std::find(list_to_find.begin(), list_to_find.end(), num_to_find) != list_to_find.end())
        return true;
    bool res = false;
    for (auto num : list_to_find)
        res = res || rec_cycles(num_to_find, num);
    return res;
}

[[maybe_unused]] bool con_component::find_cycles()
{
    bool res = false;
    for (const auto& cur_job : jobs_dep)
    {
        auto cur_num = cur_job.first;
        std::cout << "Checking (great cycle)" << cur_num << std::endl;
        auto cur_list = cur_job.second;
        for (auto num : cur_list) {
            std::cout << "Recursive " << num << std::endl;
            res = res || rec_cycles(cur_num, num);
        }
    }
    return res;
}
