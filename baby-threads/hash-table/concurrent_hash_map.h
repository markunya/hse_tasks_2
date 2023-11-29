#pragma once

#include <unordered_map>
#include <mutex>
#include <functional>
#include <vector>
#include <memory>
#include <atomic>

template <class K, class V, class Hash = std::hash<K>>
class ConcurrentHashMap {
public:
    ConcurrentHashMap(const Hash& hasher = Hash()) : ConcurrentHashMap(kUndefinedSize, hasher) {
    }

    explicit ConcurrentHashMap(int expected_size, const Hash& hasher = Hash())
        : ConcurrentHashMap(expected_size, kDefaultConcurrencyLevel, hasher) {
    }

    ConcurrentHashMap(int expected_size, int expected_threads_count, const Hash& hasher = Hash())
        : hasher_(hasher),
          data_(((expected_size <= expected_threads_count * 171)
                     ? expected_threads_count * 171
                     : ((expected_size + expected_threads_count * 171 - 1) / (expected_threads_count * 171)) *
                           expected_threads_count * 171)),
          locks_(expected_threads_count * 171) {
    }

    bool Insert(const K& key, const V& value) {
        if (Find(key).first) {
            return false;
        }
        size_t hash_of_key = hasher_(key);
        locks_[hash_of_key % locks_.size()].lock();
        if (size_ > data_.size()) {
            locks_[hash_of_key % locks_.size()].unlock();
            Rehash();
            locks_[hash_of_key % locks_.size()].lock();
        }
        size_t index = hash_of_key % data_.size();
        data_[index] = std::make_unique<Node>(key, value, std::move(data_[index]));
        ++size_;
        locks_[hash_of_key % locks_.size()].unlock();
        return true;
    }

    bool Erase(const K& key) {
        if (!Find(key).first) {
            return false;
        }
        size_t hash_of_key = hasher_(key);
        locks_[hash_of_key % locks_.size()].lock();
        size_t index = hash_of_key % data_.size();
        if (data_[index]->key == key) {
            data_[index] = std::move(data_[index]->next);
        } else {
            Node* current = data_[index].get();
            while (current->next->key != key) {
                current = current->next.get();
            }
            current->next = std::move(current->next->next);
        }
        --size_;
        locks_[hash_of_key % locks_.size()].unlock();
        return true;
    }

    void Clear() {
        for (size_t i = 0; i < locks_.size(); ++i) {
            locks_[i].lock();
            for (size_t j = i; j < data_.size(); j += locks_.size()) {
                data_[j].reset(nullptr);
            }
            locks_[i].unlock();
        }
        size_ = 0;
    }

    std::pair<bool, V> Find(const K& key) const {
        size_t hash_of_key = hasher_(key);
        locks_[hash_of_key % locks_.size()].lock();
        size_t index = hash_of_key % data_.size();
        Node* current = data_[index].get();
        while (current != nullptr && current->key != key) {
            current = current->next.get();
        }
        if (!current) {
            locks_[index % locks_.size()].unlock();
            return std::pair<bool, V>(false, V{});
        }
        V result_value = current->value;
        locks_[hash_of_key % locks_.size()].unlock();
        return std::pair<bool, V>(true, result_value);
    }

    const V At(const K& key) const {
        auto p = Find(key);
        if (!p.first) {
            throw std::out_of_range("");
        }
        return p.second;
    }

    size_t Size() const {
        return size_;
    }

    static const int kDefaultConcurrencyLevel;
    static const int kUndefinedSize;

private:
    struct Node {
        std::unique_ptr<Node> next;
        K key;
        V value;
        Node(K k, V v, std::unique_ptr<Node> n = nullptr) : next(std::move(n)), key(k), value(v) {
        }
    };

    void Rehash() {
        for (size_t i = 0; i < locks_.size(); ++i) {
            locks_[i].lock();
        }
        size_ = 0;
        std::vector<std::unique_ptr<Node>> prev_data(data_.size() * 2);
        prev_data.swap(data_);
        for (size_t i = 0; i < prev_data.size(); ++i) {
            Node* current = prev_data[i].get();
            while (current) {
                size_t index = hasher_(current->key) % data_.size();
                data_[index] =
                    std::make_unique<Node>(current->key, current->value, std::move(data_[index]));
                ++size_;
                current = current->next.get();
            }
        }
        for (size_t i = 0; i < locks_.size(); ++i) {
            locks_[i].unlock();
        }
    }

    std::atomic<size_t> size_ = 0;
    Hash hasher_;
    std::vector<std::unique_ptr<Node>> data_;
    mutable std::vector<std::mutex> locks_;
};

template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kDefaultConcurrencyLevel = 8;

template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kUndefinedSize = -1;
