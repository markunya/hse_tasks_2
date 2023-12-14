#pragma once

#include <mutex>
#include <vector>
#include <numeric>

template <class T>
class MPMCBoundedQueue {
public:
    explicit MPMCBoundedQueue(int size) : data_(size), generation_(size) {
        std::iota(generation_.begin(), generation_.end(), 0);
    }

    bool Enqueue(const T& value) {
        size_t current = back_.load();
        do {
            if (front_ == back_ && current != generation_[current].load() % data_.size()) {
                return false;
            }
        } while(!back_.compare_exchange_weak(current, (current + 1) % data_.size()));
        data_[current] = value;
        generation_[current].fetch_add(1);
        return true;
    }

    bool Dequeue(T& data) {
        size_t current = front_.load();
        do {
            if (front_ == back_ && current + 1 != generation_[current].load() % data_.size()) {
                return false;
            }
        } while(!front_.compare_exchange_weak(current, (current + 1) % data_.size()));
        data = data_[current];
        generation_[current].fetch_add(data_.size() - 1);
        return true;
    }

private:
    std::atomic<size_t> front_ = 0;
    std::atomic<size_t> back_ = 0;
    std::vector<T> data_;
    std::vector<std::atomic<size_t>> generation_;
};
