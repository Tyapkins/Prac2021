#include "parallel_threads.h"

int main() {
    //std::cout << "Pam" << std::endl;
    //auto dummy = thread_schedule();
    //dummy.gen_file_interval({1, 15}, {100, 500});
    /*auto test_graph = generate_graph(100, {10, 20}, 5, {10, 0}, {500, 3});
    auto component = test_graph.get_graph();
    auto job_filename = gen_job_filename(100, 10, 5);
    test_graph.to_out_file(job_filename);
    test_graph.to_dot(GRAPH_PATH + "test.dot");*/

    //exit(5);
    list_job thread_nums;
    for (num i = 16; i <= 16; i *= 2)
    {
        thread_nums.push_back(i);
    }
    auto new_dummy = all_threads(1);
    //new_dummy.check_coherent({100, 200}, {1, 5});
    new_dummy.check_parallel({200, 250}, thread_nums);
    //new_dummy.check_coherent({100, 150}, {1, 5});

    exit(10);
    all_threads test(1);
    test.set_jobs_name("jobs_200_2_20");
    test.set_proc_name("5_procs_200.txt");
    test.fill_data();
    test.create_threads();
    auto check = test.get_best();
    auto sol = check.first;
    for (const auto& new_set : sol)
    {
        std::cout << "[";
        for (auto num : new_set)
            std::cout << num << " ";
        std::cout << "]" << std::endl;
    }
    std::cout << "Best loss is " << check.second << std::endl;
    std::cout << "HOORAY" << std::endl;
}

