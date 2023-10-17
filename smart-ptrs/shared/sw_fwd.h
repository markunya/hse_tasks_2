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
        if (strong == 0) {
            OnZeroStrong();
            if (weak == 0) {
                OnZeroWeak();
            }
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
    ObjectWithControlBlock() : obj_() {
    }

    template <typename... Args>
    ObjectWithControlBlock(Args&&... args) : obj_(std::forward<Args>(args)...) {
    }

    T* GetPointer() {
        return &obj_;
    }

    void OnZeroStrong() override {
    }

    void OnZeroWeak() override {
        if (Strong() == 0) {
            delete this;
        }
    }

private:
    T obj_;
};

class BadWeakPtr : public std::exception {};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;
