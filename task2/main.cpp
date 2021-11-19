#include "parallel_threads.h"

int main() {
    //std::cout << "Pam" << std::endl;
    auto dummy = thread_schedule();
    dummy.gen_file_interval({1, 15}, {500, 550});

    //exit(5);
    list_job thread_nums;
    for (num i = 1; i <= 64; i *= 2)
    {
        thread_nums.push_back(i);
    }
    auto new_dummy = all_threads(1);
    //new_dummy.check_coherent({400, 450}, {1, 5});
    new_dummy.check_parallel({100, 500}, thread_nums);
    /*all_threads test(1);
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
    std::cout << "HOORAY" << std::endl;*/
}

