#pragma once

#include <utility>
#include <optional>
#include <queue>
#include <condition_variable>
#include <atomic>

template <class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size) : max_size_(size) {
    }

    void Send(const T& value) {
        auto guard = std::unique_lock{m_};
        while (q_.size() == max_size_ && !is_closed_) {
            not_full_.wait(guard);
        }
        if (is_closed_) {
            throw std::runtime_error("Attempt to send on closed chanel");
        }
        q_.push(value);
        not_empty_.notify_one();
        not_full_.notify_one();
    }

    std::optional<T> Recv() {
        auto guard = std::unique_lock{m_};
        while (q_.empty() && !is_closed_) {
            not_empty_.wait(guard);
        }
        if (q_.empty()) {
            return std::nullopt;
        }
        T result = q_.front();
        q_.pop();
        not_full_.notify_one();
        not_empty_.notify_one();
        return result;
    }

    void Close() {
        auto guard = std::unique_lock{m_};
        is_closed_ = true;
        not_empty_.notify_all();
        not_full_.notify_all();
    }

private:
    std::mutex m_;
    size_t max_size_;
    std::queue<T> q_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
    bool is_closed_ = false;
};
