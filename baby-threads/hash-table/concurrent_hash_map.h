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
          data_(((expected_size <= expected_threads_count * 7)
                     ? expected_threads_count * 7
                     : ((expected_size + expected_threads_count * 7 - 1) /
                        (expected_threads_count * 7)) *
                           expected_threads_count * 7),
                nullptr),
          locks_(expected_threads_count * 7) {
    }

    bool Insert(const K& key, const V& value) {
        if (Find(key).first) {
            return false;
        }
        rehash_mutex_.lock();
        if (size_ * 2 >= data_.size()) {
            Rehash();
        }
        rehash_mutex_.unlock();
        size_t hash_of_key = hasher_(key);
        locks_[hash_of_key % locks_.size()].lock();
        size_t index = hash_of_key % data_.size();
        data_[index] = new Node(key, value, data_[index]);
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
            auto ptr = data_[index];
            data_[index] = data_[index]->next;
            delete ptr;
        } else {
            Node* current = data_[index];
            while (current->next->key != key) {
                current = current->next;
            }
            auto ptr = current->next;
            current->next = current->next->next;
            delete ptr;
        }
        --size_;
        locks_[hash_of_key % locks_.size()].unlock();
        return true;
    }

    void Clear() {
        for (size_t i = 0; i < locks_.size(); ++i) {
            locks_[i].lock();
        }
        for (size_t i = 0; i < data_.size(); ++i) {
            if (!data_[i]) {
                continue;
            }
            data_[i]->Delete();
            data_[i] = nullptr;
        }
        size_ = 0;
        for (size_t i = 0; i < locks_.size(); ++i) {
            locks_[i].unlock();
        }
    }

    std::pair<bool, V> Find(const K& key) const {
        size_t hash_of_key = hasher_(key);
        locks_[hash_of_key % locks_.size()].lock();
        size_t index = hash_of_key % data_.size();
        Node* current = data_[index];
        while (current != nullptr && current->key != key) {
            current = current->next;
        }
        if (!current) {
            locks_[hash_of_key % locks_.size()].unlock();
            return std::pair<bool, V>(false, V{});
        }
        auto result_value = current->value;
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

    ~ConcurrentHashMap() {
        for (size_t i = 0; i < data_.size(); ++i) {
            if (!data_[i]) {
                continue;
            }
            data_[i]->Delete();
        }
    }

    static const int kDefaultConcurrencyLevel;
    static const int kUndefinedSize;

private:
    struct Node {
        Node* next;
        K key;
        V value;
        void Delete() {
            if (next != nullptr) {
                next->Delete();
            }
            delete this;
        };
        Node(K k, V v, Node* n = nullptr) : next(std::move(n)), key(k), value(v) {
        }
    };

    void Rehash() {
        for (size_t i = 0; i < locks_.size(); ++i) {
            locks_[i].lock();
        }
        std::vector<Node*> prev_data(data_.size() * 2, nullptr);
        prev_data.swap(data_);
        for (size_t i = 0; i < prev_data.size(); ++i) {
            Node* ptr = prev_data[i];
            while (ptr) {
                Node* next_ptr = ptr->next;
                size_t index = hasher_(ptr->key) % data_.size();
                ptr->next = data_[index];
                data_[index] = ptr;
                ptr = next_ptr;
            }
        }
        for (size_t i = 0; i < locks_.size(); ++i) {
            locks_[i].unlock();
        }
    }
    std::mutex rehash_mutex_;
    std::atomic<size_t> size_ = 0;
    Hash hasher_;
    std::vector<Node*> data_;
    mutable std::vector<std::mutex> locks_;
};

template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kDefaultConcurrencyLevel = 8;

template <class K, class V, class Hash>
const int ConcurrentHashMap<K, V, Hash>::kUndefinedSize = -1;
