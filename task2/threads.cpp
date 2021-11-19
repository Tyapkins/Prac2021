#include "threads.h"

bool thread_schedule::all_jobs_are_done()
{
    return std::all_of(jobs_done.begin(), jobs_done.end(), [](std::pair<std::size_t, bool> i){return i.second; });
}

std::shared_ptr<Processor> thread_schedule::get_proc_by_num(std::size_t proc_num)
{
    for (auto proc : procs)
        if (proc->get_num() == proc_num)
            return proc;
    return nullptr;
}

bool thread_schedule::can_be_done(std::size_t num)
{
    auto job = get_job_by_num(num);
    auto dep_list = job->get_depend();
    return std::all_of(dep_list.begin(), dep_list.end(), [this](std::size_t num) {return jobs_done[num];});
}

solution thread_schedule::get_config()
{
    solution new_set;
    for (const auto& proc: procs)
        new_set.insert(proc->get_job_nums());
    return new_set;
}

std::size_t thread_schedule::compute()
{
    std::size_t lose = 0;
    jobs_done.clear();
    //std::cout << "Beginning compute" << std::endl;
    //std::transform(all_jobs.begin(), all_jobs.end(), jobs_done.begin(),
    //               [](std::shared_ptr<Job> job){return std::make_pair(job->get_num(), false);});
    for (const auto& job : all_jobs)
        jobs_done.insert(std::make_pair(job->get_num(), false));
    //std::cout << "HAHAHAHAHA" << std::endl;
    jobList cur_jobs;
    done_list proc_working;
    deg_pair dur_jobs;


    fill_jobs_proc();
    for (const auto& proc : procs) {
        auto deq = proc->get_job_nums();
        auto cur_job = get_job_by_num(deq.front());
        cur_jobs.push_back(cur_job);
        if (cur_job != nullptr) {
            dur_jobs.insert(std::make_pair(proc->get_num(), cur_job->get_durat()));
            proc_working.insert(std::make_pair(proc->get_num(), cur_job->get_depend().empty()));
        }
        else {
            dur_jobs.insert(std::make_pair(proc->get_num(), 0));
            proc_working.insert(std::make_pair(proc->get_num(), false));
        }
    }
    //std::cout << "CYCLE CHECK" << std::endl;
    while (!(all_jobs_are_done()))
    {
        std::size_t min_dur = 0;
        /*for (const auto& proc : procs) {
            auto num = proc->get_num();
            if (!(proc_working[num])) {
                auto proc_jobs = proc->get_job_nums();
                if (std::all_of(proc_jobs.begin(), proc_jobs.end(), [this](std::size_t job_num){return jobs_done[job_num]; }))
                    continue;
                auto begin_job = proc_jobs.front();
                auto cur_job = begin_job;
                do
                {
                    proc->next_job();
                    cur_job = proc->get_job_nums().front();
                }
                while ((!can_be_done(cur_job)) && (cur_job != begin_job));
                if (cur_job != begin_job)
                {
                    proc_working[num] = true;
                    dur_jobs[num] = get_job_by_num(cur_job)->get_durat();
                }
            }
        }*/
        for (const auto& proc : procs) {
            if (proc_working[proc->get_num()]) {
                auto durat = dur_jobs[proc->get_num()];
                //std::cout << "Now durat is... " << durat << std::endl;
                if ((min_dur == 0) || (durat < min_dur))
                {
                    min_dur = durat;
                    //proc->get_num();
                }
            }
        }
        for (const auto& proc : procs) {
            auto proc_num = proc->get_num();
            if (proc_working[proc_num]) {
                dur_jobs[proc_num] -= min_dur;
            }
            else
                lose += min_dur;
        }
        for (const auto& proc : procs) {
            auto proc_num = proc->get_num();
            if ((proc_working[proc_num]) && (dur_jobs[proc_num] == 0))
            {
                auto proc_list = proc->get_job_nums();
                if (!jobs_done[proc_list.front()]) {
                    jobs_done[proc_list.front()] = true;
                    std::rotate(proc_list.begin(), proc_list.begin()+1, proc_list.end());
                    proc->set_job_order(proc_list);
                    //proc->next_job();
                    dur_jobs[proc_num] = get_job_by_num(proc->get_job_nums().front())->get_durat();
                }
                else
                    proc_working[proc->get_num()] = false;
            }

        }
        for (const auto& proc : procs) {
            if (!(proc->get_job_nums().empty())) {
                auto num = proc->get_job_nums().front();
                if ((can_be_done(num)) && (!jobs_done[num]))
                    proc_working[proc->get_num()] = true;
                else
                    proc_working[proc->get_num()] = false;
            }
        }
    }
    return lose;
}

solution thread_schedule::get_jobs_to_procs() {

    //solution sol;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> get_proc(0, procs.size()-1);

    std::cout << "Got proc num = " << procs.size()-1 << std::endl;

    for (const auto& proc : procs)
        proc->clear_jobs();

    job_procs.clear();

    job_set tried_procs;
    job_set tried_jobs;
    job_set need_to_change;
    bool transformed = false;
    std::size_t pos;
    std::size_t proc_num;


    for (const auto& job : all_jobs)
    {
        auto job_num = job->get_num();
        transformed = false;

        cur_all_dep.clear();
        //job_procs.clear();

        fill_jobs_proc();
        auto try_beg = tried_jobs.begin();
        auto end_beg = tried_jobs.end();
        //auto try_beg = need_to_change.begin();
        //auto end_beg = need_to_change.end();
        std::for_each(try_beg, end_beg, [this](std::size_t num){get_all_deps(num);});
        //need_to_change.clear();
        /*for (auto retry_job : tried_jobs)
            get_all_deps(retry_job);*/

        /*auto first_change = need_to_change.begin();
        auto end_change = need_to_change.end();
        std::for_each(first_change, end_change, [this](std::size_t num){get_all_deps(num);});*/

        need_to_change.clear();

        tried_procs.clear();

        while ((!transformed) && (tried_procs.size() != procs.size())) {
            for (proc_num = get_proc(gen); tried_procs.find(proc_num) != tried_procs.end(); proc_num = get_proc(gen));
            tried_procs.insert(proc_num);
            //if (job_procs.find(job_num) == job_procs.end())
                job_procs.insert(std::make_pair(job_num, proc_num));
            //else
            //    job_procs[job_num] = proc_num;
            auto actual_proc = get_proc_by_num(proc_num);
            actual_proc->add_job(job_num, 0);
            //std::cout << "Before boundaries" << std::endl;
            auto floor = get_floor_limit(proc_num, job_num), upper = get_upper_limit(proc_num, job_num);
            //std::cout << "After boundaries" << std::endl;
            actual_proc->remove_job(job_num);
            //std::cout << "Boundaries are " << floor << " and " << upper << std::endl;
            upper = (upper == 0) ? 0 : upper-1;
            floor = (floor == 0) ? 0 : floor-1;
            if (floor > upper) {
                job_procs.erase(job_num);
                continue;
            }
            std::uniform_int_distribution<> get_pos(floor, upper);
            pos = get_pos(gen);
            actual_proc -> add_job(job_num, pos);
            transformed = true;
            tried_procs.clear();
        }
        /*auto job_deq = get_proc_by_num(proc_num)->get_job_nums();
        auto iter = std::find(job_deq.begin(), job_deq.end(), job_num);
        need_to_change.insert(iter+1, job_deq.end());
        auto mast_job = find_slaves(job_num);
        //mast_job.insert()
        for (auto new_job : mast_job)
            if (tried_jobs.find(new_job) != tried_jobs.end())
                need_to_change.insert(new_job);
            //need_to_change.insert(mast_job.begin(), mast_job.end());
        //std::cout <<  "Done job num = " << job_num << " at proc " << proc_num << " on " << pos << std::endl;*/
        tried_jobs.insert(job_num);

    }

    return get_config();
}

job_set thread_schedule::find_slaves(std::size_t num)
{
    job_set masters;
    for (const auto& job : all_jobs)
    {
        auto dep = job->get_depend();
        if (dep.find(num) != dep.end())
            masters.insert(job->get_num());
    }
    return masters;
}

job_set thread_schedule::get_chain_dep(std::size_t num)
{
    job_set all_dep;
    if (visited.find(num) != visited.end())
        return cur_all_dep[num];
    visited.insert(num);
    auto actual_job = get_job_by_num(num);
    auto expl_dep = actual_job->get_depend();
    auto proc_jobs = get_proc_by_num(get_proc_by_job_num(num))->get_job_nums();
    all_dep.insert(expl_dep.begin(), expl_dep.end());
    auto begin_it = proc_jobs.begin();
    auto end_it = proc_jobs.end();
    auto pos_it = std::find(begin_it, end_it, num);
    if (pos_it != end_it)
        all_dep.insert(begin_it, pos_it);
    job_set jobs_to_add;
    auto help_fun = [this, &jobs_to_add](std::size_t num) mutable {
        if (visited.find(num) == visited.end()) {
            auto help_set = get_all_deps(num);
            jobs_to_add.insert(help_set.begin(), help_set.end());
        }
    };
    std::for_each(all_dep.begin(), all_dep.end(), help_fun);
    all_dep.insert(jobs_to_add.begin(), jobs_to_add.end());
    cur_all_dep.insert(std::make_pair(num, all_dep));
    return all_dep;
}

std::size_t thread_schedule::get_floor_limit(std::size_t proc_num, std::size_t job_num)
{
    auto proc_nums = get_proc_by_num(proc_num)->get_job_nums();
    auto job_dep = get_all_deps(job_num);
    auto beg_it = proc_nums.rbegin();
    auto end_it = proc_nums.rend();
    auto f = [job_dep](std::size_t num)
            {return job_dep.find(num) != job_dep.end();};
    auto last_it = std::find_if(beg_it, end_it, f);
    return end_it-last_it;
}

job_set thread_schedule::get_all_deps(std::size_t num)
{
    if (cur_all_dep.find(num) != cur_all_dep.end())
        return cur_all_dep[num];
    else {
        visited.clear();
        return get_chain_dep(num);
    }
}

std::size_t thread_schedule::get_upper_limit(std::size_t proc_num, std::size_t job_num)
{
    auto proc_nums = get_proc_by_num(proc_num)->get_job_nums();
    auto beg_it = proc_nums.begin();
    auto end_it = proc_nums.end();

    auto f = [this, job_num](std::size_t num)
    {
        if (num != job_num)
        {
            auto dep = get_all_deps(num);
            auto dep_beg = dep.begin();
            auto dep_end = dep.end();
            auto g = [dep, job_num](std::size_t num) {return num == job_num;};
            return std::find_if(dep_beg, dep_end, g) != dep_end;
        }
        else
            return false;
    };
    auto last_it = std::find_if(beg_it, end_it, f);
    return last_it-beg_it;
}

bool thread_schedule::operation_1(std::size_t job_num, std::size_t new_proc_num)
{
    std::size_t old_proc_num = get_proc_by_job_num(job_num);

    if (old_proc_num == new_proc_num)
        return false;

    auto old_proc = get_proc_by_num(old_proc_num);
    auto new_proc = get_proc_by_num(new_proc_num);

    std::size_t floor_lim = get_floor_limit(new_proc_num, job_num), upper_lim = get_upper_limit(new_proc_num, job_num);

    if (floor_lim > upper_lim)
        return false;

    old_proc -> remove_job(job_num);

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> get_pos(floor_lim, upper_lim);

    auto num_pos = get_pos(gen);
    new_proc->add_job(job_num, num_pos);
    job_procs.erase(job_num);
    job_procs.insert(std::make_pair(job_num, new_proc_num));

    return true;
}

bool thread_schedule::operation_2(std::size_t job_num)
{
    auto proc_num = get_proc_by_job_num(job_num);

    auto proc = get_proc_by_num(proc_num);

    auto proc_deq = proc->get_job_nums();

    std::size_t old_pos = (std::find(proc_deq.begin(), proc_deq.end(), job_num) - proc_deq.begin());

    proc->remove_job(job_num);

    std::size_t floor_lim = get_floor_limit(proc_num, job_num), upper_lim = get_upper_limit(proc_num, job_num);

    if (floor_lim >= upper_lim) {
        proc->add_job(job_num, old_pos);
        return false;
    }


    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> get_pos(floor_lim, upper_lim);

    std::size_t new_pos;
    for (new_pos = get_pos(gen); new_pos == old_pos; new_pos = get_pos(gen));

    proc->add_job(job_num, new_pos);

    return true;
}

solution thread_schedule::transform_solution()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> get_op(1, 2);
    std::uniform_int_distribution<> get_job(0, all_jobs.size()-1);
    std::uniform_int_distribution<> get_proc(0, procs.size()-1);

    auto op_num = get_op(gen);

    cur_all_dep.clear();

    bool transformed = false;
    job_set tried_jobs, all_procs;

    fill_jobs_proc();

    while ((!transformed) && (tried_jobs.size() != all_jobs.size())) {
        auto job_num = get_job(gen);
        //std::cout << "And what job is it? It is " << job_num << std::endl << std::endl;
        char tried_all = 0;
        tried_jobs.insert(job_num);
        op_num = get_op(gen);
        all_procs.clear();
        while ((tried_all < 2) && (!transformed))
        {
            //std::cout << "Trying number " << op_num << ", Indicator = " << int(tried_all) << std::endl;
            if (op_num == 1) {
                std::size_t proc_num = get_proc(gen);
                for (; (!operation_1(job_num, proc_num)) && (all_procs.size() != procs.size()); proc_num = get_proc(gen)) {
                    all_procs.insert(proc_num);
                    //std::cout << " " << proc_num << std::endl;
                }
                if (all_procs.size() != procs.size())
                    transformed = true;
                else {
                    tried_all++;
                    op_num = 2;
                }
            }
            else if (op_num == 2) {
                if (!operation_2(job_num)) {
                    tried_all++;
                    op_num = 1;
                }
                else
                    transformed = true;
            }
        }
        //std::cout << "Iteration!!!" << std::endl;
    }

    /*if (!transformed) {
        std::cout << "Transformations can not be done! Stop iterating." << std::endl;
        for (const auto& num : get_config())
        {
            for (auto pam : num)
                std::cout << pam << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
    }*/
    return get_config();

}

void thread_schedule::set_config(const solution& config)
{
    if (config.size() != procs.size()) {
        return;
    }
    std::size_t counter = 0;
    for (const auto& proc: procs) {
        proc->clear_jobs();
    }
    for (const auto& deq : config)
    {
        procs[counter++]->set_job_order(deq);
    }
}

std::shared_ptr<Job> thread_schedule::get_job_by_num(std::size_t num) {
    for (auto job : all_jobs)
        if (job->get_num() == num)
            return job;
    return nullptr;
}

solution thread_schedule::iterate(std::size_t iteration_num)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> res(0, 1);
    std::size_t loss;

    solution old_config;

    for (std::size_t i = 0; i < iteration_num; i++)
    {
        //std::cout << "Pam" << std::endl;
        loss = compute();
        //std::cout << "Why" << std::endl;
        if (loss < best_loss)
        {
            set_loss(loss);
            set_best(get_config());
        }
        else
        {
            auto num = res(gen);
            auto p = exp((double(best_loss)-loss)/Temp);
            if (num < p)
                set_config(get_config());
            else
                set_config(old_config);
        }
        old_config = get_config();

        transform_solution();
    }
    std::cout << "Iterations done, change temperature" << std::endl;
    return get_best();
}

void thread_schedule::fill_jobs_proc()
{
    job_procs.clear();
    for (const auto& proc : procs) {
        auto deq = proc->get_job_nums();
        for (auto job : deq)
            job_procs.insert(std::make_pair(job, proc->get_num()));
    }
}

solution thread_schedule::full_cycle(std::size_t cycle_num, std::size_t iteration_num)
{
    solution sol;
    double begin_temp = Temp;
    for (int counter = 0; counter < cycle_num; counter++) {
        sol = iterate(iteration_num);
        temp_change(begin_temp, counter+1);
    }
    return sol;
}

double thread_schedule::temp_change(double T0, double iter_num)
{
    Temp = T0/log(1+iter_num);
    return Temp;
}

void thread_schedule::create_procs(std::size_t proc_num)
{
    procs.clear();
    for (int i = 0; i < proc_num; i++)
    {
        auto new_p = std::make_shared<Processor>(i);
        procs.push_back(new_p);
    }
}

jobList thread_schedule::generate_job_file(std::size_t node_num,
                                        interval weight_int = {50, 10000},
                                        std::size_t weight_disp = 2000,
                                        parameters components = {2, 0.2},
                                        parameters degrees = {20, 3},
                                        const std::string& filename = "new_test.txt")
{
    auto test_graph = generate_graph(node_num, weight_int, weight_disp, components, degrees);
    auto component = test_graph.get_graph();
    auto job_filename = gen_job_filename(node_num, components.first, degrees.first);
    test_graph.to_out_file(job_filename);
    test_graph.to_dot(GRAPH_PATH + "graph_" + std::to_string(node_num) + ".dot");
    return read_jobs(job_filename);
}

procList thread_schedule::generate_proc_file(std::size_t proc_num,
                                     std::size_t node_num = 100,
                                     const std::string& job_filename = "jobs.txt") {

    all_jobs = std::move(read_jobs(job_filename));
    create_procs(proc_num);
    get_jobs_to_procs();
    procs_to_file(procs, gen_proc_filename(proc_num, node_num));
    return procs;
}

void thread_schedule::generate_files(std::size_t proc_num,
                    std::size_t node_num = 100,
                    interval weight_int = {50, 10000},
                    std::size_t weight_disp = 2000,
                    parameters components = {2, 0.5},
                    parameters degrees = {30, 25},
                    const std::string& filename = "new_test.txt")
{
    auto generated_jobs = generate_job_file(node_num, weight_int, weight_disp, components, degrees);
    generate_proc_file(proc_num, node_num, gen_job_filename(node_num, components.first, degrees.first));
}

void thread_schedule::gen_file_interval(interval proc_interval, interval node_interval,
                                        num proc_diff, num node_diff,
                                        num comp_num, num deg_num)
{
    for (num node_counter = node_interval.first; node_counter < node_interval.second+1; node_counter += node_diff)
    {
        generate_job_file(node_counter);
        for (num proc_counter = proc_interval.first; proc_counter < proc_interval.second+1; proc_counter += proc_diff)
        {
            generate_proc_file(proc_counter, node_counter, gen_job_filename(node_counter, comp_num, deg_num));
        }
    }
}
solution thread_schedule::do_anneal(std::size_t proc_num,
               std::size_t node_num = 100,
               std::size_t cycle_num = 20,
               std::size_t iteration_num = 20,
               interval weight_int = {50, 10000},
               std::size_t weight_disp = 2000,
               parameters components = {2, 0.2},
               parameters degrees = {20, 3},
               const std::string& filename = "new_test.txt")
{
    generate_files(proc_num, node_num, weight_int, weight_disp, components, degrees);
    return full_cycle(cycle_num, iteration_num);
}

/*int main()
{
    //auto pam = read_jobs("jobs.txt");
    //auto procs = read_procs("schedule.txt");
    //thread_schedule check(pam, procs);

    thread_schedule new_thread;

    new_thread.do_anneal(5);

    /*solution tyap = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {}, {}};

    check.set_config(tyap);

    check.fill_jobs_proc();

    check.operation_2(3);*/

    //interval weight_int(50, 10000);
    //parameters components(2, 0.2);
    //parameters degrees(20, 3);
    //auto pam = generate_graph(1000, weight_int, 2000, components, degrees);
    //auto checking = pam.get_graph();
    //std::size_t num = 0;
    //pam.to_out_file("super_new.txt");
    //auto all_jobs = read_jobs("super_new.txt");
    //thread_schedule sched(all_jobs);
    //sched.create_procs(10);
    //std::cout << "why" << std::endl;
    //solution test = {{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}};
    //sched.fill_jobs_proc();
    /*sched.set_config(test);*/
    //sched.get_jobs_to_procs();
    //std::cout << "help" << std::endl;
    /*auto fisj = sched.get_config();
    for (auto num : fisj)
        for (auto namnum : num)
            std::cout << namnum << " ";
        std::cout << std::endl;*/
    //auto best_sol = sched.full_cycle(20, 10);
    //for (auto job : all_jobs)
    //    std::cout << job->get_durat() << std::endl;

    //std::cout << std::endl << std::endl;
    /*for (const auto& proc : best_sol) {
        std::cout << "{";
        for (auto purumpam : proc)
            std::cout << purumpam << ", ";
        std::cout << "};" << std::endl << std::endl;
    }*/

//    return 0;
//}
