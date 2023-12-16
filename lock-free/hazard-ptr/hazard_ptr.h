#pragma once

#include <atomic>
#include <functional>
#include <shared_mutex>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <thread>

extern thread_local std::atomic<void*> hazard_ptr;
extern std::shared_mutex threads_lock;

void RegisterThread();

void UnregisterThread();

void Release();

template <class T>
T* Acquire(std::atomic<T*>* ptr) {
    auto value = ptr->load();
    do {
        hazard_ptr.store(value);
        auto new_value = ptr->load();
        if (new_value == value) {
            return value;
        }
        value = new_value;
    } while (true);
}

void ScanFreeList();

struct RetiredPtr {
    void* value;
    std::function<void(void*)> deleter;
    RetiredPtr* next;
    RetiredPtr(void* value, std::function<void(void*)> deleter, RetiredPtr* next = nullptr);
};

void FreeListPush(RetiredPtr* ptr);

static const int kFreeListScanSize = 10;
extern std::atomic<RetiredPtr*> free_list;
extern std::atomic<int> approximate_free_list_size;

template <class T, class Deleter = std::default_delete<T>>
void Retire(T* ptr) {
    auto lambda_deleter = [](void* ptr) {
        Deleter deleter;
        deleter(reinterpret_cast<T*>(ptr));
    };
    auto new_free_list = new RetiredPtr(ptr, lambda_deleter, free_list.load());
    FreeListPush(new_free_list);
    approximate_free_list_size.fetch_add(1);
    if (approximate_free_list_size.load() > kFreeListScanSize) {
        ScanFreeList();
    }
}
