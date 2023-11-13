#pragma once

#include <utility>
#include <functional>

template <typename Signature>
class FunctionRef;

template <typename R, typename... Args>
class FunctionRef<R(Args...)> {
public:
    template <typename F>
    FunctionRef(F&& f) {
        Builder(f);
    }

    template <typename F>
    FunctionRef& operator=(F&& f) {
        Builder(f);
        return *this;
    }

    R operator()(Args... args) const {
        return static_cast<R>(springboard_(callback_, std::forward<Args>(args)...));
    }

private:
    void* callback_ = nullptr;
    R (*springboard_)(void*, Args...) = nullptr;

    template <class F>
    void Builder(F& f) {
        callback_ = reinterpret_cast<void*>(std::addressof(f));
        springboard_ = [](void* callback, Args... args) {
            return static_cast<R>(std::invoke(
                *reinterpret_cast<typename std::add_pointer_t<F>>(callback),
                std::forward<Args>(args)...));
        };
    }

    template <typename R_, typename... Args_>
    void Builder(R_ (*f)(Args_...)) {
        callback_ = reinterpret_cast<void*>(f);
        springboard_ = [](void* callback, Args... args) {
            return static_cast<R>(
                std::invoke(reinterpret_cast<decltype(f)>(callback), std::forward<Args>(args)...));
        };
    }
};
