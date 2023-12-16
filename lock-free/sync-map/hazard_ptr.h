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

struct RetiredPtr {
    void* value;
    std::function<void(void*)> deleter;
    RetiredPtr* next;
    RetiredPtr(void* value, std::function<void(void*)> deleter, RetiredPtr* next = nullptr);
};

class FreeList {
public:
    static const size_t kFreeListScanSize = 10;

public:
    size_t Size();
    void Push(RetiredPtr* ptr);
    void ScanFreeList();
    ~FreeList();

private:
    std::mutex scan_lock_;
    std::atomic<size_t> size_{0};
    std::atomic<RetiredPtr*> head_ = nullptr;
};

extern FreeList free_list;

template <class T, class Deleter = std::default_delete<T>>
void Retire(T* ptr) {
    auto lambda_deleter = [](void* ptr) {
        Deleter deleter;
        deleter(reinterpret_cast<T*>(ptr));
    };
    auto new_head = new RetiredPtr(ptr, lambda_deleter, nullptr);
    free_list.Push(new_head);
    if (free_list.Size() > FreeList::kFreeListScanSize) {
        free_list.ScanFreeList();
    }
}
