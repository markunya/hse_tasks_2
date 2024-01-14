#include <executors.h>

Task::Task() {
    static std::atomic<size_t> prev_id{1};
    id_ = prev_id.load();
    while (!prev_id.compare_exchange_weak(id_, id_ + 1)) {
    }
}

void Task::AddDependency(std::shared_ptr<Task> task) {
    task->dependent_on_me_.emplace_back(shared_from_this());
    need_dependencies_.fetch_add(1);
    can_be_done_.store(false);
}

void Task::AddTrigger(std::shared_ptr<Task> task) {
    task->triggered_by_me_.emplace_back(shared_from_this());
    can_be_done_.store(false);
}

void Task::SetTimeTrigger(std::chrono::system_clock::time_point at) {
    deadline_ = at;
    can_be_done_.store(false);
}

bool Task::IsCompleted() {
    return is_completed_.load();
}

bool Task::IsFailed() {
    return is_failed_.load();
}

bool Task::IsCanceled() {
    return is_canceled_.load();
}

bool Task::IsFinished() {
    return IsFailed() || IsCanceled() || IsCompleted();
}

std::exception_ptr Task::GetError() {
    return error_;
}

void Task::Cancel() {
    bool t = false;
    if (is_canceled_.compare_exchange_strong(t, true)) {
        for (auto task : dependent_on_me_) {
            task->need_dependencies_.fetch_sub(1);
            if (task->need_dependencies_.load() == 0) {
                task->can_be_done_.store(true);
            }
        }
        for (auto task : triggered_by_me_) {
            task->can_be_done_.store(true);
        }
        auto guard = std::unique_lock{global};
        auto task_guard = std::unique_lock{task_mutex_};
        have_job.notify_one();
        done_.notify_all();
    }
}

void Task::Wait() {
    auto task_guard = std::unique_lock{task_mutex_};
    while (!IsFinished()) {
        //done_.wait(task_guard);
    }
}

bool Task::SubNeedDependencies() {
    size_t prev = need_dependencies_.load();
    do {
        if (prev == 0) {
            throw std::runtime_error("Sub when zero dependencies are needed");
        }
    } while (!need_dependencies_.compare_exchange_weak(prev, prev - 1));
    if (need_dependencies_.load() == 0) {
        return true;
    }
    return false;
}

void Task::SetWhenFinished(std::chrono::system_clock::time_point at) {
}

Task::~Task() = default;

Executor::Executor(size_t num_of_threads) {
    workers_.reserve(num_of_threads);
    for (size_t i = 0; i < workers_.capacity(); ++i) {
        workers_.emplace_back(&Executor::TakeTask, this);
    }
}

void Executor::Submit(std::shared_ptr<Task> task) {
    auto guard = std::unique_lock{Task::global};
    if (task->can_be_done_.load() ||
        (task->deadline_ < std::chrono::system_clock::now() &&
         task->deadline_ != std::numeric_limits<std::chrono::system_clock::time_point>::min())) {
        ready_.push(task);
        Task::have_job.notify_one();
        return;
    }
    if (task->deadline_ != std::numeric_limits<std::chrono::system_clock::time_point>::min()) {
        timer_heap_.push(task);
        Task::have_job.notify_one();
    }
    tasks_.emplace_back(task);
    Task::have_job.notify_one();
}

void Executor::StartShutdown() {
    is_shut_downed_.store(true);
    auto guard = std::unique_lock{Task::global};
    while (!ready_.empty()) {
        ready_.pop();
    }
    while (!timer_heap_.empty()) {
        timer_heap_.pop();
    }
    tasks_.clear();
    Task::have_job.notify_all();
}

void Executor::WaitShutdown() {
    auto guard = std::unique_lock{Task::global};
    while (shut_downed_threads_ < workers_.size()) {
        shutdown_complete_.wait(guard);
    }
}

void Executor::TakeTask() {
    auto guard = std::unique_lock{Task::global};
    while (!is_shut_downed_.load()) {
        std::shared_ptr<Task> task;
        while (!timer_heap_.empty() &&
               (timer_heap_.top()->deadline_ < std::chrono::system_clock::now() ||
                timer_heap_.top()->can_be_done_.load())) {
            timer_heap_.top()->can_be_done_.store(true);
            timer_heap_.pop();
        }
        auto it = tasks_.begin();
        while (it != tasks_.end()) {
            auto current = it++;
            if ((*current)->can_be_done_) {
                auto task_mb_next = (*current);
                tasks_.erase(current);
                ready_.push(task_mb_next);
                Task::have_job.notify_one();
            }
        }
        if (!ready_.empty()) {
            task = ready_.front();
            ready_.pop();
        }
        if (!task) {
            shut_downed_threads_.fetch_add(1);
            if (!timer_heap_.empty()) {
                auto at = timer_heap_.top()->deadline_;
                Task::have_job.wait_until(guard, at);
            } else {
                Task::have_job.wait(guard);
            }
            shut_downed_threads_.fetch_sub(1);
            continue;
        }
        guard.unlock();
        DoTask(task);
        guard.lock();
    }
    shut_downed_threads_.fetch_add(1);
    if (shut_downed_threads_ == workers_.size()) {
        shutdown_complete_.notify_all();
    }
}

void Executor::DoTask(std::shared_ptr<Task> task) {
    if (task->is_canceled_.load()) {
        NotifyTask(task);
        return;
    }
    try {
        task->Run();
    } catch (...) {
        task->error_ = std::current_exception();
        task->is_failed_.store(true);
        NotifyTask(task);
        return;
    }
    task->is_completed_.store(true);
    NotifyTask(task);
}

void Executor::NotifyTask(std::shared_ptr<Task> task) {
    {
        auto task_guard = std::unique_lock{task->task_mutex_};
        task->done_.notify_all();
    }
    auto guard = std::unique_lock{Task::global};
    for (auto i : task->triggered_by_me_) {
        if (i->can_be_done_.load()) {
            continue;
        }
        i->can_be_done_.store(true);
        i->triggered_by_.store(task->id_);
        Task::have_job.notify_one();
    }
    task->triggered_by_me_.clear();
    for (auto i : task->dependent_on_me_) {
        if (i->can_be_done_.load()) {
            continue;
        }
        if (i->SubNeedDependencies()) {
            i->can_be_done_.store(true);
            Task::have_job.notify_one();
        }
    }
    task->dependent_on_me_.clear();
    task->SetWhenFinished(std::chrono::system_clock::now());
}

Executor::~Executor() {
    StartShutdown();
    for (auto& worker : workers_) {
        worker.join();
    }
}

std::shared_ptr<Executor> MakeThreadPoolExecutor(int num_threads) {
    return std::make_shared<Executor>(num_threads);
}
