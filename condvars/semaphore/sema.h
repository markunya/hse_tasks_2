#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>

class DefaultCallback {
public:
    void operator()(int& value) {
        --value;
    }
};

class Semaphore {
public:
    Semaphore(int count) : count_(count) {
    }

    void Leave() {
        std::unique_lock<std::mutex> lock(mutex_);
        ++count_;
        q_.pop();
        cv_.notify_all();
    }

    template <class Func>
    void Enter(Func callback) {
        std::unique_lock<std::mutex> lock(mutex_);
        q_.emplace(std::this_thread::get_id());
        while (!count_ || q_.front() != std::this_thread::get_id()) {
            cv_.wait(lock);
        }
        callback(count_);
    }

    void Enter() {
        DefaultCallback callback;
        Enter(callback);
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<std::thread::id> q_;
    int count_ = 0;
};
