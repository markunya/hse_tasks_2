#pragma once

#include <stack>

template <typename It, typename P, typename F>
void TransformIf(It begin, It end, P p, F f) {
    std::stack<std::pair<It, typename std::iterator_traits<It>::value_type>> log;
    for (auto it = begin; it != end; ++it) {
        try {
            if (p(*it)) {
                try {
                    log.emplace(it, *it);
                } catch (...) {
                }
                f(*it);
            }
        } catch (...) {
            while (!log.empty()) {
                try {
                    *(log.top().first) = log.top().second;
                    log.pop();
                } catch (...) {
                }
            }
            throw;
        }
    }
}