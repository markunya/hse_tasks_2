#include "is_prime.h"
#include <cmath>
#include <algorithm>
#include <thread>
#include <vector>
#include <shared_mutex>

struct Flag {
public:
    void Set() {
        std::unique_lock lock{m_};
        flag_ = true;
    }

    bool Get() const {
        std::shared_lock lock{m_};
        return flag_;
    }

private:
    mutable std::shared_mutex m_;
    bool flag_ = false;
};

void IsPrimeChecker(uint64_t number, uint64_t root, uint64_t index, uint64_t num_of_threads,
                    Flag* flag) {
    for (uint64_t i = index + 2; i <= root; i += num_of_threads) {
        if (flag->Get()) {
            return;
        }
        if (number % i == 0) {
            flag->Set();
            return;
        }
    }
}

bool IsPrime(uint64_t x) {
    if (x <= 1) {
        return false;
    }
    Flag result;
    uint64_t root = sqrt(x);
    uint64_t num_of_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> workers;
    for (uint64_t i = 0; i < num_of_threads; ++i) {
        workers.emplace_back(IsPrimeChecker, x, root, i, num_of_threads, &result);
    }
    for (uint64_t i = 0; i < num_of_threads; ++i) {
        workers[i].join();
    }
    return !result.Get();
}
