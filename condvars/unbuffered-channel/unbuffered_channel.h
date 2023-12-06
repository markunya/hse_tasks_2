#pragma once

#include <utility>
#include <optional>
#include <condition_variable>

template <class T>
class UnbufferedChannel {
public:
    void Send(const T& value) {
        auto guard = std::unique_lock{m_};
        while (is_sending_ && !is_closed_) {
            can_send_.wait(guard);
        }
        if (is_closed_) {
            throw std::runtime_error("Attempt to send on closed channel");
        }
        is_sending_ = true;
        current_value_ = value;
        sent_.notify_one();
        while (current_value_.has_value() && !is_closed_) {
            received_.wait(guard);
        }
        is_sending_ = false;
        can_send_.notify_one();
        if (current_value_.has_value()) {
            current_value_ = std::nullopt;
            throw std::runtime_error("Attempt to send on closed channel");
        }
    }

    std::optional<T> Recv() {
        auto guard = std::unique_lock{m_};
        while (is_receiving_ && !is_closed_) {
            can_receive_.wait(guard);
        }
        if (is_closed_) {
            return std::nullopt;
        }
        is_receiving_ = true;
        while (!current_value_.has_value() && !is_closed_) {
            sent_.wait(guard);
        }
        if (is_closed_) {
            return std::nullopt;
        }
        auto result = current_value_;
        current_value_ = std::nullopt;
        is_receiving_ = false;
        can_receive_.notify_one();
        received_.notify_one();
        return result;
    }

    void Close() {
        auto guard = std::unique_lock{m_};
        is_closed_ = true;
        received_.notify_all();
        can_send_.notify_all();
        sent_.notify_all();
        can_receive_.notify_all();
    }

private:
    bool is_closed_ = false;
    bool is_sending_ = false;
    bool is_receiving_ = false;
    std::mutex m_;
    std::optional<T> current_value_ = std::nullopt;
    std::condition_variable received_;
    std::condition_variable sent_;
    std::condition_variable can_send_;
    std::condition_variable can_receive_;
};
