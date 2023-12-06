#pragma once
#include <mutex>
#include <condition_variable>

class RWLock {
public:
    template <class Func>
    void Read(Func func) {
        {
            auto guard = std::unique_lock{global_};
            while (amount_of_writers_waiting_ > 0 || is_writer_active_) {
                cd_.wait(guard);
            }
            ++amount_of_readers_active_;
        }
        try {
            func();
        } catch (...) {
            EndRead();
            throw;
        }
        EndRead();
    }

    template <class Func>
    void Write(Func func) {
        {
            auto guard = std::unique_lock{global_};
            ++amount_of_writers_waiting_;
            while (amount_of_readers_active_ > 0 || is_writer_active_) {
                cd_.wait(guard);
            }
            --amount_of_writers_waiting_;
            is_writer_active_ = true;
        }
        try {
            func();
        } catch (...) {
            EndWrite();
            throw;
        }
        EndWrite();
    }

private:
    size_t amount_of_writers_waiting_ = 0;
    size_t amount_of_readers_active_ = 0;
    bool is_writer_active_ = false;
    std::condition_variable cd_;
    std::mutex global_;

    void EndRead() {
        auto guard = std::unique_lock{global_};
        --amount_of_readers_active_;
        if (amount_of_readers_active_ == 0) {
            cd_.notify_all();
        }
    }

    void EndWrite() {
        auto guard = std::unique_lock{global_};
        is_writer_active_ = false;
        cd_.notify_all();
    }
};
