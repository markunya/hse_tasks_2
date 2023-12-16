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

RetiredPtr::RetiredPtr(void *value, std::function<void(void *)> deleter, RetiredPtr *next) {
}

std::atomic<RetiredPtr *> free_list = nullptr;
std::atomic<int> approximate_free_list_size = 0;

void FreeListPush(RetiredPtr *ptr) {
    while (!free_list.compare_exchange_weak(ptr->next, ptr)) {
    }
}

std::mutex scan_lock;

void ScanFreeList() {
    if (!approximate_free_list_size.exchange(0)) {
        return;
    }
    auto scan_guard = std::unique_lock<std::mutex>{scan_lock};
    // (0) Обнуляем approximate_free_list_size, чтобы другие потоки не пытались зайти в ScanFreeList
    // вместе с нами.

    // (1) С помощью мьютекса убеждаемся, что не больше одного потока занимается сканированием.
    // В реальном коде не забудьте использовать guard.

    // (2) Забираем все указатели из free_list
    RetiredPtr *retired = free_list.exchange(nullptr);

    // (3) Читаем множество защищённых указателей обойдя все ThreadState.
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
            FreeListPush(current);
        }
    }
    // (4) Сканируем все retired указатели.
    //    (a) Для тех, что не находятся в hazard, вызываем деструктор и освобождаем память под
    //    RetiredPtr. (b) Те, что еще находятся в hazard, кладём назад в free_list.
    //
    // Для ускорения, hazard нужно посортировать и использовать бинарный поиск.
}
