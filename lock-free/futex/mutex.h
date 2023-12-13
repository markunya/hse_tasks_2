#pragma once

#include <linux/futex.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>
#include <atomic>

void FutexWait(int *value, int expected_value) {
    syscall(SYS_futex, value, FUTEX_WAIT_PRIVATE, expected_value, nullptr, nullptr, 0);
}

void FutexWake(int *value, int count) {
    syscall(SYS_futex, value, FUTEX_WAKE_PRIVATE, count, nullptr, nullptr, 0);
}

class Mutex {
public:
    void Lock() {
        int t = CompareExchange(&atom_, 0, 1);
        if (t != 0) {
            do {
                if (t == 2 || CompareExchange(&atom_, 1, 2) != 0) {
                    FutexWait(reinterpret_cast<int *>(&atom_), 2);
                }
            } while ((t = CompareExchange(&atom_, 0, 2)) != 0);
        }
    }

    void Unlock() {
        if (atom_.fetch_sub(1) != 1) {
            atom_.store(0);
            FutexWake(reinterpret_cast<int *>(&atom_), 1);
        }
    }

private:
    std::atomic<int> atom_{0};

    int CompareExchange(std::atomic<int> *a, int expected, int desired) {
        int *ep = &expected;
        std::atomic_compare_exchange_strong(a, ep, desired);
        return *ep;
    }
};
