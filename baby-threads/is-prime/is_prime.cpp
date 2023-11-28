#include "is_prime.h"
#include <cmath>
#include <algorithm>
#include <thread>
#include <vector>
#include <shared_mutex>
#include <mutex>
#include <atomic>

void IsPrimeChecker(uint64_t number, uint64_t root, uint64_t index, uint64_t num_of_threads,
                    std::atomic<bool>* flag) {
    for (uint64_t i = index + 2; i <= root; i += num_of_threads) {
        if (*flag) {
            return;
        }
        if (number % i == 0) {
            *flag = true;
            return;
        }
    }
}

bool IsPrime(uint64_t x) {
    if (x <= 1) {
        return false;
    }
    std::atomic<bool> result = false;
    uint64_t root = sqrt(x);
    uint64_t num_of_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> workers;
    for (uint64_t i = 0; i < num_of_threads; ++i) {
        workers.emplace_back(IsPrimeChecker, x, root, i, num_of_threads, &result);
    }
    for (uint64_t i = 0; i < num_of_threads; ++i) {
        workers[i].join();
    }
    return !result;
}
