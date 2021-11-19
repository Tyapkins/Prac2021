#include <fstream>
#include <iomanip>
#include "graph.h"


Graph generate_graph(size_t node_num,
                     interval weights, double weight_disp,
                     parameters con_components_par,
                     parameters degree_par)
{
    static std::random_device rd{};
    static std::mt19937 gen{rd()};

    std::normal_distribution<> get_weight {double(weights.second-weights.first)/2, sqrt(weight_disp)};
    std::normal_distribution<> degree_get {degree_par.first, sqrt(degree_par.second)};

    std::vector<size_t> nodenum_comp;
    double num = 0;
    size_t weight = 0;
    size_t whole_num = 0;

    jobList jobs;
    full_graph graph;

    auto min_deg = (((degree_par.first == 0) || (node_num < con_components_par.first)) ? 0 : 1);

    for (int i = 0; i < node_num; i++)
    {
        for (num = get_weight(gen) + weights.first;
            (num <= 0) || (num > weights.second);
             num = get_weight(gen) + weights.first);
        weight = std::round(num);
        for (num = degree_get(gen); num < min_deg; num = degree_get(gen));
        Job job_check(i, weight, std::round(num));
        auto job_ptr = std::make_shared<Job>(job_check);
        jobs.push_back(job_ptr);
    }

    std::normal_distribution<> component {double(node_num)/con_components_par.first, sqrt(con_components_par.second)};
    int counter = 0;

    weight = 0;
    for (int i = node_num; i > 0; i -= std::round(num))
    {
        for (num = component(gen); (num < 1) || (num > node_num); num = component(gen));
        whole_num = (i > num) ? std::round(num) : i;
        jobList sub_job(jobs.begin()+weight, jobs.begin()+weight+whole_num);
        con_component new_comp(counter++, sub_job);
        new_comp.regulate_degrees();
        new_comp.generate_depend();
        new_comp.fill_all_dependencies();
        graph.push_back(new_comp);
        weight += whole_num;
    }
    Graph final_graph(node_num, graph);

    return final_graph;
}

std::string Graph::num_to_name(std::size_t num, u_char first_sym = u'a', std::size_t alph_size = 26)
{
    std::string name;
    for (int i = num; i > 0; i /= alph_size)
        name += u_char(first_sym+i%alph_size);
    name = name.empty() ? "a" : name;
    return name;
}

[[maybe_unused]] void Graph::to_dot(const std::string& filename = "graph.dot")
{
    std::ofstream dot_file;
    dot_file.open(filename, std::ios::out | std::ios::trunc);
    std::size_t con_num = 0;
    dot_file << "digraph full_graph {" << std::endl;
    job_set added;
    for (auto con : components) {
        dot_file << std::setw(4) << ("subgraph component_" + std::to_string(con_num++) + " {") << std::endl;
        auto depends = con.get_dependencies();
        std::string head_label, tail_label;
        for (const auto& master_job : depends) {
            tail_label.clear();
            if (added.find(master_job.first) == added.end())
            {
                tail_label = std::to_string(con.get_job_by_num(master_job.first)->get_durat());
                added.insert(master_job.first);
            }
            for (auto dep_job : master_job.second) {
                head_label.clear();
                if (added.find(dep_job) == added.end()) {
                    head_label = std::to_string(con.get_job_by_num(dep_job)->get_durat());
                    added.insert(dep_job);
                }
                dot_file << std::setw(8) << num_to_name(master_job.first) << " -> " << num_to_name(dep_job);// <<
                if (!head_label.empty()) {
                    dot_file << R"([headlabel=<<font color="red"><B>)" << head_label << "</B></font>>";
                    if (!tail_label.empty())
                        dot_file << R"(, taillabel=<<font color="red"><B>)" << tail_label << "</B></font>>";
                    dot_file << "]";
                }
                else if (!tail_label.empty())
                    dot_file << R"([taillabel=<<font color="red"><B>)" << tail_label << "</B></font>>""]";
                dot_file << ";" << std::endl;
                tail_label.clear();
            }
            if (master_job.second.empty())
                dot_file << std::setw(8) << num_to_name(master_job.first) << "[xlabel=<<font color='red'><B>)" << tail_label << "</B></font>>""]"
                << ";" << std::endl << std::endl;
        }
        dot_file << std::setw(4) << "}" << std::endl << std::endl;
    }
    dot_file << "}" << std::endl << std::endl;
}

jobList Graph::get_all_jobs()
{
    jobList all_jobs;
    for (auto comp: components)
    {
        for (const auto& job : comp.get_jobs())
            if (std::find(all_jobs.begin(), all_jobs.end(), job) == all_jobs.end())
                all_jobs.push_back(job);
    }
    return all_jobs;
}

void Graph::to_out_file(const std::string& filename)
{
    jobs_to_file(get_all_jobs(), filename);
}

