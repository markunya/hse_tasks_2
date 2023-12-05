#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <map>

template <class T>
class TimerQueue {
public:
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;

public:
    void Add(const T& item, TimePoint at) {
        auto guard = std::unique_lock{m_};
        data_.emplace(at, item);
        on_pop_.notify_one();
    }

    T Pop() {
        auto guard = std::unique_lock{m_};
        while (data_.empty()) {
            on_pop_.wait(guard);
        }
        on_pop_.wait_until(guard, data_.begin()->first);
        T result = std::move(data_.begin()->second);
        data_.erase(data_.begin());
        return result;
    }

private:
    std::mutex m_;
    std::condition_variable on_pop_;
    std::multimap<TimePoint, T> data_;
};
