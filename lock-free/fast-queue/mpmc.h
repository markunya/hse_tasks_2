#pragma once

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
            if (current != generation_[current % Size()].load()) {
                return false;
            }
        } while (!back_.compare_exchange_weak(current, current + 1));
        data_[current % Size()] = value;
        generation_[current % Size()].fetch_add(1);
        return true;
    }

    bool Dequeue(T& data) {
        size_t current = front_.load();
        do {
            if (current + 1 != generation_[current % Size()].load()) {
                return false;
            }
        } while (!front_.compare_exchange_weak(current, current + 1));
        data = data_[current % Size()];
        generation_[current % Size()].exchange(current + Size());
        return true;
    }

private:
    std::atomic<size_t> front_ = 0;
    std::atomic<size_t> back_ = 0;
    std::vector<T> data_;
    std::vector<std::atomic<size_t>> generation_;

    size_t Size() {
        return data_.size();
    }
};
