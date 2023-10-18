#pragma once

#include <exception>

struct ControlBlockBase {
    size_t strong = 1;
    size_t weak = 0;

    virtual void OnZeroStrong() = 0;

    virtual void OnZeroWeak() = 0;

    size_t Weak() {
        return weak;
    }

    size_t Strong() {
        return strong;
    }

    void DecStrong() {
        --strong;
        if (weak == 0 && strong == 0) {
            OnZeroStrong();
            OnZeroWeak();
        } else if (strong == 0) {
            OnZeroStrong();
        }
    }

    void DecWeak() {
        --weak;
        if (weak == 0 && strong == 0) {
            OnZeroWeak();
        }
    }

    void IncStrong() {
        ++strong;
    }

    void IncWeak() {
        ++weak;
    }

    virtual ~ControlBlockBase() = default;
    ControlBlockBase() = default;
};

template <typename T>
class ControlBlock : public ControlBlockBase {
public:
    ControlBlock() = default;
    ControlBlock(T* ptr) : ptr_(ptr) {
    }

    void OnZeroStrong() override {
        delete ptr_;
    }

    void OnZeroWeak() override {
        delete this;
    }

private:
    T* ptr_ = nullptr;
};

template <typename T>
class ObjectWithControlBlock : public ControlBlockBase {
public:
    ObjectWithControlBlock() {
        new (&buffer_) T();
    }

    template <typename... Args>
    ObjectWithControlBlock(Args&&... args) {
        new (&buffer_) T(std::forward<Args>(args)...);
    }

    T* GetPointer() {
        return reinterpret_cast<T*>(&buffer_);
    }

    void OnZeroStrong() override {
        T* x = GetPointer();
        (*x).~T();
    }

    void OnZeroWeak() override {
        if (Strong() == 0) {
            delete this;
        }
    }

private:
    std::aligned_storage_t<sizeof(T), alignof(T)> buffer_;
};

class BadWeakPtr : public std::exception {};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;
