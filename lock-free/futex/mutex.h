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
        int t = 0;
        if (!a_.compare_exchange_strong(t, 1)) {
            do {
                if (t == 2 || a_.compare_exchange_strong(t, 2)) {
                    FutexWait(reinterpret_cast<int *>(&a_), 2);
                }
            } while (a_.compare_exchange_strong(t, 2));
        }
    }

    void Unlock() {
        if (a_.fetch_sub(1) != 1) {
            a_.store(0);
            FutexWake(reinterpret_cast<int *>(&a_), 1);
        }
    }

private:
    std::atomic<int> a_ = 0;
};
