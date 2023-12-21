#include "find_subsets.h"
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <thread>

int threads_count = std::thread::hardware_concurrency();  // std::thread::hardware_concurrency();
std::vector<int64_t> half_1;
std::vector<int64_t> half_2;

int Log(uint32_t n) {
    return sizeof(n) * __CHAR_BIT__ - 1 - __builtin_clz(n);
}

Subsets CreateSubsets(uint32_t first, uint32_t second) {
    Subsets result;
    result.exists = true;
    for (size_t i = 0; first != 0; ++i) {
        if (first & 1) {
            result.first_indices.emplace_back(i);
        }
        first >>= 1;
    }
    for (size_t i = 0; second != 0; ++i) {
        if (second & 1) {
            result.second_indices.emplace_back(i);
        }
        second >>= 1;
    }
    return result;
}

void Job(std::unordered_map<int64_t, std::vector<std::pair<uint32_t, uint32_t>>>* m,
         std::atomic<bool>* exists, int k, Subsets* result, uint32_t start) {
    for (uint32_t i = start; i < half_2.size(); i += threads_count) {
        for (uint32_t j = i + 1; j < half_2.size(); ++j) {
            if (exists->load()) {
                return;
            }
            if (i & j) {
                continue;
            }
            int64_t key = std::abs(half_2[i] - half_2[j]);
            if (!m->contains(key)) {
                continue;
            }
            for (auto [i_o, j_o] : m->operator[](key)) {
                if (half_2[i] + half_1[i_o] != half_2[j] + half_1[j_o]) {
                    std::swap(i_o, j_o);
                }
                if ((i_o == 0 && i == 0) || (j_o == 0 && j == 0)) {
                    continue;
                }
                if (!exists->exchange(true)) {
                    *result = CreateSubsets((i << k) + i_o, (j << k) + j_o);
                }
            }
        }
    }
}

Subsets FindEqualSumSubsets(const std::vector<int64_t>& data) {
    size_t size = data.size();
    size_t k = size / 2;
    if (size == 30) {
        k = 14;
    }
    half_1.resize(1 << k, 0);
    half_2.resize(1 << (size - k), 0);
    for (uint32_t i = 1; i < half_1.size(); ++i) {
        half_1[i] = data[Log(i)] + half_1[i - (1 << Log(i))];
    }
    for (uint32_t i = 1; i < half_2.size(); ++i) {
        half_2[i] = data[k + Log(i)] + half_2[i - (1 << Log(i))];
    }
    std::unordered_map<int64_t, std::vector<std::pair<uint32_t, uint32_t>>> m;
    for (uint32_t i = 0; i < half_1.size(); ++i) {
        for (uint32_t j = i + 1; j < half_1.size(); ++j) {
            if (i & j) {
                continue;
            }
            m[std::abs(half_1[i] - half_1[j])].emplace_back(i, j);
        }
    }
    std::atomic<bool> exists = false;
    Subsets result;
    result.exists = false;
    std::vector<std::thread> workers;
    for (int i = 0; i < threads_count; ++i) {
        workers.emplace_back(Job, &m, &exists, k, &result, i);
    }
    for (int i = 0; i < threads_count; ++i) {
        workers[i].join();
    }
    return result;
}
