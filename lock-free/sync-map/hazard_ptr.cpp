#include "hazard_ptr.h"

thread_local std::atomic<void *> hazard_ptr{nullptr};
std::shared_mutex threads_lock;
std::unordered_map<std::thread::id, std::atomic<void *> *> registered_threads;

void RegisterThread() {
    auto guard = std::unique_lock<std::shared_mutex>{threads_lock};
    registered_threads[std::this_thread::get_id()] = &hazard_ptr;
}

void UnregisterThread() {
    auto guard = std::unique_lock<std::shared_mutex>{threads_lock};
    registered_threads.erase(std::this_thread::get_id());
}

void Release() {
    hazard_ptr.store(nullptr);
}

RetiredPtr::RetiredPtr(void *value, std::function<void(void *)> deleter, RetiredPtr *next)
    : value(value), deleter(deleter), next(next) {
}

FreeList free_list;

size_t FreeList::Size() {
    return size_.load();
}

void FreeList::Push(RetiredPtr *ptr) {
    while (!head_.compare_exchange_weak(ptr->next, ptr)) {
    }
}

void FreeList::ScanFreeList() {
    if (!size_.exchange(0)) {
        return;
    }
    auto scan_guard = std::unique_lock<std::mutex>{scan_lock_};
    RetiredPtr *retired = head_.exchange(nullptr);
    std::unordered_set<void *> hazard;
    {
        std::unique_lock guard(threads_lock);
        for (auto [id, atom_ptr] : registered_threads) {
            auto ptr = atom_ptr->load();
            if (ptr) {
                hazard.insert(ptr);
            }
        }
    }
    while (retired) {
        auto current = retired;
        current->next = nullptr;
        retired = retired->next;
        if (hazard.contains(current->value)) {
            current->deleter(current->value);
            delete current;
        } else {
            free_list.Push(current);
        }
    }
}

FreeList::~FreeList() {
    auto current = head_.load();
    while (current) {
        auto ptr = current;
        current = current->next;
        ptr->deleter(ptr->value);
        delete ptr;
    }
}
