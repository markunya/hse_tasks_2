#pragma once
#include <thread>

template <class RandomAccessIterator, class T, class Func>
void ReduceForThread(RandomAccessIterator first, RandomAccessIterator last, Func func, int step,
                     T* result) {
    if (first >= last) {
        return;
    }
    *result = *first;
    while (first + step < last) {
        first += step;
        *result = func(*result, *(first));
    }
}

template <class RandomAccessIterator, class T, class Func>
T Reduce(RandomAccessIterator first, RandomAccessIterator last, const T& initial_value, Func func) {
    auto cur_value(initial_value);
    size_t step = std::thread::hardware_concurrency();
    if (step >= std::distance(first, last)) {
        while (first != last) {
            cur_value = func(cur_value, *(first++));
        }
        return cur_value;
    }
    T workers_results[step];
    std::vector<std::thread> workers;
    for (size_t i = 0; i < step; ++i) {
        workers.emplace_back(ReduceForThread<RandomAccessIterator, T, Func>, first + i, last, func,
                             step, &workers_results[i]);
    }
    for (size_t i = 0; i < step; ++i) {
        workers[i].join();
    }
    for (size_t i = 0; i < step; ++i) {
        cur_value = func(cur_value, workers_results[i]);
    }
    return cur_value;
}
