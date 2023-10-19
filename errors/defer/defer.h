#pragma once

#include <algorithm>
#include <functional>

template <typename Callback>
class CallbackStorage {
public:
    explicit CallbackStorage(Callback callback) {
        ::new (GetCallbackBuffer()) Callback(std::move(callback));
    }

    void* GetCallbackBuffer() {
        return static_cast<void*>(callback_buffer_);
    }

    Callback& GetCallback() {
        return *reinterpret_cast<Callback*>(GetCallbackBuffer());
    }

private:
    alignas(Callback) char callback_buffer_[sizeof(Callback)];
};

template <typename Callback>
class Defer final {
public:
    Defer(Callback callback) : storage_(std::move(callback)) {
    }

    void Cancel() {
        if (!is_canceled_) {
            storage_.GetCallback().~Callback();
            is_canceled_ = true;
        }
    }

    void Invoke() noexcept {
        if (!is_canceled_) {
            try {
                std::move(storage_.GetCallback())();
                Cancel();
            } catch (...) {
            }
        }
    }

    ~Defer() {
        Invoke();
    }

private:
    bool is_canceled_ = false;
    CallbackStorage<Callback> storage_;
};
