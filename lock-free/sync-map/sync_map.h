#pragma once

#include <atomic>
#include <unordered_map>
#include <memory>
#include <mutex>

#include "hazard_ptr.h"

template <class K, class V>
class SyncMap {
public:
    SyncMap()
        : snapshot_{new Snapshot()}, mutable_map_(std::make_shared<std::unordered_map<K, V>>()) {
    }

    bool Lookup(const K& key, V* value) {
        auto snapshot = Acquire(&snapshot_);
        if (snapshot->read_only->contains(key)) {
            *value = snapshot->read_only->operator[](key);
            Release();
            return true;
        }
        Release();
        return false;
    }

    bool Insert(const K& key, const V& value) {
        auto guard = std::scoped_lock<std::mutex>{lock_};
        if (snapshot_.load()->read_only->contains(key)) {
            mutable_map_->operator[](key) = value;
            Update();
            return false;
        }
        mutable_map_->operator[](key) = value;
        Update();
        return true;
    }

    ~SyncMap() {
        delete snapshot_.load();
    }

private:
    struct Snapshot {
        Snapshot() : read_only(std::make_shared<std::unordered_map<K, V>>()) {
        }
        Snapshot(const std::unordered_map<K, V>& m)
            : read_only(std::make_shared<std::unordered_map<K, V>>(m)) {
        }
        Snapshot* Copy() {
            return (new Snapshot(*read_only));
        }
        std::shared_ptr<std::unordered_map<K, V>> read_only;
    };

    void Update() {
        Snapshot* new_snapshot = snapshot_.load()->Copy();
        for (auto& [key, val] : (*mutable_map_)) {
            new_snapshot->read_only->operator[](key) = val;
        }
        Snapshot* old_snapshot = snapshot_.exchange(new_snapshot);
        if (old_snapshot) {
            Retire(old_snapshot);
        }
        mutable_map_->clear();
    }

    std::atomic<Snapshot*> snapshot_;
    std::mutex lock_;
    std::shared_ptr<std::unordered_map<K, V>> mutable_map_;
};
