#pragma once

#include <atomic>

struct RWSpinLock {
public:
    void LockRead() {
        while (true) {
            int prev = counter_.load();
            if (!(prev & 1)) {
                if (counter_.compare_exchange_strong(prev, prev + 2)) {
                    break;
                }
            }
        }
    }

    void UnlockRead() {
        counter_.fetch_sub(2);
    }

    void LockWrite() {
        while (true) {
            int prev = counter_.load();
            if (!prev) {
                if (counter_.compare_exchange_strong(prev, 1)) {
                    break;
                }
            }
        }
    }

    void UnlockWrite() {
        counter_.fetch_sub(1);
    }

private:
    std::atomic<int> counter_{0};
};
