#include <memory>
#include <chrono>
#include <vector>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <list>
#include <optional>

class Task : public std::enable_shared_from_this<Task> {
public:
    Task();

    virtual void Run() = 0;

    // Task::run() completed without throwing exception
    bool IsCompleted();

    // Task::run() throwed exception
    bool IsFailed();

    // Task was Canceled
    bool IsCanceled();

    // Task either completed, failed or was Canceled
    bool IsFinished();

    std::exception_ptr GetError();

    void Wait();

    void Cancel();

    void AddDependency(std::shared_ptr<Task> task);

    void AddTrigger(std::shared_ptr<Task> task);

    void SetTimeTrigger(std::chrono::system_clock::time_point at);

    virtual ~Task();

private:
    friend class Executor;

    bool SubNeedDependencies();

    virtual void SetWhenFinished(std::chrono::system_clock::time_point at);

    std::mutex task_mutex_;
    std::condition_variable done_;
    inline static std::mutex global;
    inline static std::condition_variable have_job;

    size_t id_ = 0;
    std::atomic<size_t> triggered_by_{0};
    std::atomic<size_t> need_dependencies_ = 0;
    std::atomic<bool> can_be_done_{true};
    std::atomic<bool> is_completed_{false};
    std::atomic<bool> is_failed_{false};
    std::atomic<bool> is_canceled_{false};
    std::vector<std::shared_ptr<Task>> dependent_on_me_;
    std::vector<std::shared_ptr<Task>> triggered_by_me_;
    std::exception_ptr error_ = nullptr;
    std::chrono::system_clock::time_point deadline_ =
        std::numeric_limits<std::chrono::system_clock::time_point>::min();
};

template <class T>
class Future;

template <class T>
using FuturePtr = std::shared_ptr<Future<T>>;

// Used instead of void in generic code
struct Unit {};

class Executor {
public:
    Executor(size_t num_of_threads = std::thread::hardware_concurrency());

    void Submit(std::shared_ptr<Task> task);

    void StartShutdown();
    void WaitShutdown();

    template <class T>
    FuturePtr<T> Invoke(std::function<T()> fn) {
        auto task = std::make_shared<Future<T>>();
        task->Set(fn);
        Submit(task);
        return std::shared_ptr<Future<T>>(task);
    }

    template <class Y, class T>
    FuturePtr<Y> Then(FuturePtr<T> input, std::function<Y()> fn) {
        auto task = std::make_shared<Future<Y>>();
        task->Set(fn);
        task->AddDependency(input);
        Submit(task);
        return task;
    }

    template <class T>
    FuturePtr<std::vector<T>> WhenAll(std::vector<FuturePtr<T>> all) {
        auto task = std::make_shared<Future<std::vector<T>>>();
        for (auto dep : all) {
            task->AddDependency(dep);
        }
        task->Set([all]() -> std::vector<T> {
            std::vector<T> res;
            res.reserve(all.size());
            for (auto f : all) {
                res.emplace_back(f->Get());
            }
            return res;
        });
        Submit(task);
        return task;
    }

    template <class T>
    FuturePtr<T> WhenFirst(std::vector<FuturePtr<T>> all) {
        auto task = std::make_shared<Future<T>>();
        for (auto tr : all) {
            task->AddTrigger(tr);
        }
        task->Set([all, task]() {
            for (auto tr : all) {
                if (tr->id_ == task->triggered_by_) {
                    return tr->Get();
                }
            }
        });
        Submit(task);
        return task;
    }

    template <class T>
    FuturePtr<std::vector<T>> WhenAllBeforeDeadline(
        std::vector<FuturePtr<T>> all, std::chrono::system_clock::time_point deadline) {
        auto task = std::make_shared<Future<std::vector<T>>>();
        task->SetTimeTrigger(deadline);
        task->Set([all, deadline]() {
            std::vector<T> res;
            for (auto f : all) {
                if (f->finished_at_ < deadline) {
                    res.emplace_back(f->Get());
                }
            }
            return res;
        });
        Submit(task);
        return task;
    }

    ~Executor();

private:
    void NotifyTask(std::shared_ptr<Task> task);

    void TakeTask();

    void DoTask(std::shared_ptr<Task> task);

    std::atomic<bool> is_shut_downed_{false};
    std::atomic<uint8_t> shut_downed_threads_{0};
    std::condition_variable shutdown_complete_;
    std::vector<std::thread> workers_;
    std::queue<std::shared_ptr<Task>, std::list<std::shared_ptr<Task>>> ready_;
    std::priority_queue<std::shared_ptr<Task>, std::vector<std::shared_ptr<Task>>,
                        decltype([](std::shared_ptr<Task> first, std::shared_ptr<Task> second) {
                            return first->deadline_ > second->deadline_;
                        })>
        timer_heap_;
    std::list<std::shared_ptr<Task>> tasks_;
};

std::shared_ptr<Executor> MakeThreadPoolExecutor(int num_threads);

template <class T>
class Future : public Task {
public:
    T Get();

private:
    friend class Executor;

    void Set(std::function<T()> fn);

    void Run() override;

    void SetWhenFinished(std::chrono::system_clock::time_point at) override;

    std::mutex m_;
    std::function<T()> fn_;

    std::condition_variable data_loaded_;
    std::chrono::system_clock::time_point finished_at_ =
        std::chrono::time_point<std::chrono::system_clock>::max();
    std::optional<T> data_ = std::nullopt;
    std::exception_ptr error_ = nullptr;
};

template <class T>
T Future<T>::Get() {
    Wait();
    if (error_) {
        std::rethrow_exception(error_);
    }
    return data_.value();
}

template <typename T>
void Future<T>::Set(std::function<T()> fn) {
    fn_ = std::move(fn);
}

template <typename T>
void Future<T>::Run() {
    try {
        data_ = std::invoke(fn_);
    } catch (...) {
        error_ = std::current_exception();
    }
    data_loaded_.notify_all();
}

template <class T>
void Future<T>::SetWhenFinished(std::chrono::system_clock::time_point at) {
    finished_at_ = at;
}
