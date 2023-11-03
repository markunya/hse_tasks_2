#pragma once

#include <constexpr_map.h>
#include <cstddef>
#include <type_traits>

template <class K, class V, int S>
constexpr auto Sort(ConstexprMap<K, V, S> map) {
    auto result = map;
    for (size_t i = 0; i < result.Size(); ++i) {
        for (size_t j = 0; j + 1 < result.Size(); ++j) {
            if constexpr (std::is_integral_v<K>) {
                if (result.GetByIndex(j).first < result.GetByIndex(j + 1).first) {
                    std::pair<K, V> x = result.GetByIndex(j);
                    result.Erase(x.first);
                    result[x.first] = x.second;
                }
            } else {
                if (result.GetByIndex(j + 1).first < result.GetByIndex(j).first) {
                    std::pair<K, V> x = result.GetByIndex(j);
                    result.Erase(x.first);
                    result[x.first] = x.second;
                }
            }
        }
    }
    return result;
}
