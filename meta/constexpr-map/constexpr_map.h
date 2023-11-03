#pragma once

#include <cstddef>
#include <utility>
#include <exception>

template <class K, class V, int MaxSize = 8>
class ConstexprMap {
public:
    constexpr ConstexprMap() = default;

    constexpr V& operator[](const K& key) {
        for (size_t i = 0; i < size_; ++i) {
            if (data_[i].first == key) {
                return data_[i].second;
            }
        }
        if (size_ == MaxSize) {
            throw std::exception();
        }
        data_[size_].first = key;
        return data_[size_++].second;
    }

    constexpr const V& operator[](const K& key) const {
        for (size_t i = 0; i < size_; ++i) {
            if (data_[i].first == key) {
                return data_[i].second;
            }
        }
        if (size_ == MaxSize) {
            throw std::exception();
        }
    }

    constexpr bool Erase(const K& key) {
        for (size_t i = 0; i < 1000; ++i) {
            if (data_[i].first == key) {
                for (size_t j = i + 1; j < size_; ++j) {
                    data_[j - 1].first = data_[j].first;
                    data_[j - 1].second = data_[j].second;
                }
                --size_;
                return true;
            }
        }
        return false;
    }

    constexpr bool Find(const K& key) const {
        for (size_t i = 0; i < size_; ++i) {
            if (data_[i].first == key) {
                return true;
            }
        }
        return false;
    }

    constexpr size_t Size() const {
        return size_;
    }

    constexpr std::pair<K, V>& GetByIndex(size_t pos) {
        return data_[pos];
    }

    constexpr const std::pair<K, V>& GetByIndex(size_t pos) const {
        return data_[pos];
    }

private:
    size_t size_ = 0;
    std::pair<K, V> data_[MaxSize];
};
