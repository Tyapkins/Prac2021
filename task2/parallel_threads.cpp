#include "parallel_threads.h"

void run_thread::initialize(const std::string& jobs_name, const std::string& proc_name) {
    obj.set_procs(read_procs(proc_name));
    obj.set_jobs(read_jobs(jobs_name));
    obj.fill_jobs_proc();
}

void all_threads::fill_data() {
    par_threads.clear();
    all_solutions.clear();
    for (int i = 0; i < thread_num; i++)
    {
        run_thread new_thr;
        new_thr.initialize(jobs_name, proc_name);
        auto check_config = new_thr.get_config();
        while (all_solutions.find(check_config) != all_solutions.end())
            check_config = new_thr.transform_solution();
        all_solutions.insert(check_config);
        par_threads.push_back(std::make_shared<run_thread>(new_thr));
    }
}

bool all_threads::all_finished()
{
    for (const auto &job : job_finished)
        if (!job.second)
            return false;
    return true;
}

bool all_threads::all_iterated()
{
    for (const auto &job : job_pause)
        if (!job.second)
            return false;
    return true;
}

double all_threads::create_threads() {
    std::size_t counter = 0;
    job_pause.clear();
    job_finished.clear();
    generated = false;
    for (const auto& obj : par_threads) {
        obj->set_num(counter++);
        job_pause.insert(std::make_pair(obj->get_num(), false));
        job_finished.insert(std::make_pair(obj->get_num(), false));
        obj->set_thread(std::thread(cycle_with_saving, this, obj->get_num()));
    }
    generated = true;

    auto beg_time = std::chrono::high_resolution_clock::now();

    std::cout << "Checks" << std::endl;

    while (!all_finished()) {
        std::cout << "Begin exchanging" << std::endl;
        exchange_solutions();
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    std::cout << "Done exchanging" << std::endl;

    for (const auto& obj : par_threads) {
        obj->get_thread()->join();
    }

    return std::chrono::duration<double, std::deci>(end_time-beg_time).count();
}

solution all_threads::hard_change(std::shared_ptr<run_thread> thr_obj)
{
    auto old_config = thr_obj->get_config();
    auto set_sol = get_all_solution();
    while (set_sol.find(old_config) != set_sol.end())
    {
        thr_obj->transform_solution();
        old_config = thr_obj->get_config();
        if (thr_obj->is_all_tried())
            break;
    }
    add_to_solutions(old_config);
    return old_config;
}

solution all_threads::easy_change(std::shared_ptr<run_thread> thr_obj)
{
    //auto old_config = thr_obj->get_config();
    thr_obj->transform_solution();
    return thr_obj->get_config();
}

solution all_threads::cycle_with_saving(all_threads* point_to_obj, std::size_t obj_num)
{

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> res(0, 1);
    std::size_t loss;

    while (!point_to_obj->is_generated())
    {};

    /*
    //auto* actual_args = (args_for_thread*)str_arg;
    std::size_t actual_num = actual_args->num;
    //std::cout << "Actual num is " << actual_num << std::endl;
    auto point_to_obj = actual_args->point;
    auto thr_obj = point_to_obj->get_thread_by_num(actual_num);*/
    auto thr_obj = point_to_obj->get_thread_by_num(obj_num);

    double begin_temp = thr_obj->get_temp();
    solution old_config;
    //std::cout << "Okay we're here so what now" << std::endl;
    for (std::size_t counter = 0; counter < thr_obj->get_cycle_num(); counter++) {
        //std::cout << "Cycling 1 to obj = " << obj_num << std::endl;
        for (std::size_t iter_num = 0; iter_num < thr_obj->get_iter_num(); iter_num++)
        {
            //std::cout << "Cyclying 2" << std::endl;
            loss = thr_obj->compute();
            //std::cout << "See" << std::endl;
            //std::cout << "Computing breaks all?" << std::endl;
            if (loss < thr_obj->get_best_loss())
            {
                thr_obj->set_loss(loss);
                thr_obj->set_best(thr_obj->get_config());
            }
            else
            {
                auto num = res(gen);
                auto p = exp((double(thr_obj->get_best_loss()-loss)/thr_obj->get_temp()));
                if (num < p)
                    thr_obj->set_config(thr_obj->get_config());
                else
                    thr_obj->set_config(old_config);
            }
            //std::cout << "Got it" << std::endl;
            while (point_to_obj->is_transforming())
            {};

            point_to_obj->begin_transform();

            //std::cout << "help" << std::endl;

            /*old_config = thr_obj->get_config();
            auto set_sol = point_to_obj->get_all_solution();
            //std::cout << "Solution is long" << std::endl;
            while (set_sol.find(old_config) != set_sol.end())
            {
                thr_obj->transform_solution();
                old_config = thr_obj->get_config();
                set_sol = point_to_obj->get_all_solution();
            }
            //std::cout << "bottleneck" << std::endl;
            point_to_obj->add_to_solutions(old_config);*/
            point_to_obj->hard_change(thr_obj);

            point_to_obj->end_transform();

        }
        thr_obj->temp_change(begin_temp, counter+1);
        if (counter%(thr_obj->get_div()) == 0) {
            //std::cout << "hHOORAY" << std::endl;
            point_to_obj->pause_job(obj_num);
            //point_to_obj->exchange_solutions();
            while (point_to_obj->is_paused(obj_num))
            {
                //std::this_thread::yield();
            };
        }
        while (point_to_obj->is_exchanging())
        {};
    }
    //std::cout << "Why?" << std::endl;

    point_to_obj->finish_job(obj_num);
    //delete actual_args;
    return thr_obj->get_best();
}

std::shared_ptr<run_thread> all_threads::get_thread_by_num(std::size_t num)
{
    return *(par_threads.begin()+num);
}

loss_and_sol all_threads::exchange_solutions()
{
    //std::cout << "We're here!" << std::endl;
    while ((!all_iterated()))
    {
        if (all_finished())
            return std::make_pair(best_solut, best_lose);
        //std::cout << "SO WHAT" << std::endl;
    }

    //std::cout << "SO WHAT" << std::endl;

    begin_exchange();

    //std::cout << "Iterated all!" << std::endl;
    loss_and_sol best_sol;
    best_sol.second = INTMAX_MAX;

    improved_solution sol_with_time;

    for (const auto& obj : par_threads) {
        auto cur_loss = obj->get_best_loss();
        if (cur_loss < best_sol.second)
        {
            best_sol.first = obj->get_best();
            best_sol.second = cur_loss;
            sol_with_time = obj->get_improved_config();
        }
    }

    best_solut = best_sol.first;
    best_lose = best_sol.second;

    best_impr = sol_with_time;

    //std::cout << "FIRST CYCLE d IS DONE" << std::endl;
    for (const auto& obj : par_threads) {
        obj->set_config(best_sol.first);
        obj->set_best(best_sol.first);
        obj->set_loss(best_sol.second);
        obj->set_improved(best_impr);
        unpause_job(obj->get_num());
    }

    end_exchange();
    std::cout << "Iterated a cycle" << std::endl;
    return best_sol;
}

void all_threads::check_coherent(interval node_num, interval proc_num, num iter_num, num node_diff, num proc_diff)
{
    std::ofstream out_file, best_solution;
    //out_file.open("coherent_res.txt", std::ios::out | std::ios::trunc);
    std::string prefix = "coherent_res/coherent_res_";
    for (num node_counter = node_num.first; node_counter < node_num.second+1; node_counter += node_diff)
    {
        set_jobs_name(gen_job_filename(node_counter));
        for (num proc_counter = proc_num.first; proc_counter < proc_num.second+1; proc_counter += proc_diff)
        {
            out_file.open(prefix+std::to_string(node_counter) + "_" + std::to_string(proc_counter) + ".txt");
            set_proc_name(gen_proc_filename(proc_counter, node_counter));
            out_file << node_counter << ";" << proc_counter << std::endl;
            for (num iter = 0; iter < iter_num; iter++)
            {
                fill_data();
                out_file << create_threads()/10 << " ";
                std::cout << "ITERATED!!!" << std::endl << std::endl << std::endl;
            }
            out_file << std::endl << std::endl;
            out_file.close();
            best_solution.open("best_coherent/best_sol_" + std::to_string(node_counter) + "_" + std::to_string(proc_counter) + ".txt");
            for (const auto& set_with_time : best_impr)
            {
                best_solution << "[";
                for (auto num : set_with_time.first)
                    best_solution << num << " ";
                best_solution << "] : " << set_with_time.second << std::endl;
            }
            best_solution << "Best loss is " << best_lose << std::endl;
            best_solution.close();
        }
    }
}

void all_threads::check_parallel(interval node_num, const list_job& thread_nums, num iter_num, num node_diff, num proc_num)
{
    std::ofstream out_file, best_solution;
    std::string prefix = "parallel_res/parallel_res_";
    //out_file.open("parallel_res.txt", std::ios::out | std::ios::trunc);

    for (num node_counter = node_num.first; node_counter < node_num.second+1; node_counter += node_diff)
    {
        set_jobs_name(gen_job_filename(node_counter));
        set_proc_name(gen_proc_filename(proc_num, node_counter));
        for (auto thread_counter : thread_nums)
        {
            out_file.open(prefix+std::to_string(node_counter) + "_" + std::to_string(thread_counter) + ".txt");
            set_thread_num(thread_counter);
            out_file << node_counter << ";" << thread_counter << std::endl;
            for (num iter = 0; iter < iter_num; iter++)
            {
                fill_data();
                out_file << create_threads()/10 << " ";
            }
            out_file << std::endl << std::endl;
            out_file.close();
            best_solution.open("best_parallel/best_sol_" + std::to_string(node_counter) + "_" + std::to_string(thread_counter) + ".txt");
            for (const auto& set_with_time : best_impr)
            {
                best_solution << "[";
                for (auto num : set_with_time.first)
                    best_solution << num << " ";
                best_solution << "] : " << set_with_time.second << std::endl;
            }
            best_solution << "Best loss is " << best_lose << std::endl;
            best_solution.close();
        }
    }
}