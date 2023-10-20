#pragma once

#include <exception>
#include <stdexcept>
#include <cstring>

template <class T>
class Try {
public:
    Try() {
    }
    explicit Try(const T& x) : obj_(x) {
    }
    template <typename E>
    explicit Try(const E& e) : e_(std::make_exception_ptr(e)) {
    }
    explicit Try(std::exception_ptr e_ptr) : e_(e_ptr) {
    }
    T Value() const {
        if (static_cast<bool>(e_)) {
            std::rethrow_exception(e_);
        }
        if (!obj_.has_value()) {
            throw std::runtime_error("Object is empty");
        }
        return obj_.value();
    }

    void Throw() {
        if (!static_cast<bool>(e_)) {
            throw std::runtime_error("No exception");
        }
        std::rethrow_exception(e_);
    };

    bool IsFailed() const {
        return static_cast<bool>(e_);
    }

private:
    std::optional<T> obj_;
    std::exception_ptr e_ = nullptr;
};

template <>
class Try<void> {
public:
    Try() {
    }
    template <typename E>
    explicit Try(const E& e) : e_(std::make_exception_ptr(e)) {
    }
    explicit Try(std::exception_ptr e_ptr) : e_(e_ptr) {
    }

    void Throw() {
        if (!static_cast<bool>(e_)) {
            throw std::runtime_error("No exception");
        }
        std::rethrow_exception(e_);
    };

    bool IsFailed() const {
        return static_cast<bool>(e_);
    }

private:
    std::exception_ptr e_ = nullptr;
};

template <class Function, class... Args>
auto TryRun(Function func, Args... args) {
    using ReturnType = decltype(func(args...));
    if constexpr (std::is_same_v<ReturnType, void>) {
        try {
            func(args...);
            return Try<void>();
        } catch (const char* error) {
            return Try<void>(std::make_exception_ptr(std::runtime_error(error)));
        } catch (int error) {
            if (error == 2) {
                return Try<ReturnType>(
                    std::make_exception_ptr(std::runtime_error("No such file or directory")));
            }
            return Try<void>(std::make_exception_ptr(std::strerror(error)));
        } catch (const std::exception& e) {
            return Try<void>(std::current_exception());
        } catch (...) {
            return Try<void>(std::make_exception_ptr(std::runtime_error("Unknown exception")));
        }
    } else {
        try {
            return Try<ReturnType>(func(args...));
        } catch (const char* error) {
            return Try<ReturnType>(std::make_exception_ptr(std::runtime_error(error)));
        } catch (int error) {
            return Try<ReturnType>(std::make_exception_ptr(std::strerror(error)));
        } catch (const std::exception& e) {
            return Try<ReturnType>(std::current_exception());
        } catch (...) {
            return Try<ReturnType>(
                std::make_exception_ptr(std::runtime_error("Unknown exception")));
        }
    }
}