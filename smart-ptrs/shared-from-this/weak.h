#pragma once

#include "sw_fwd.h"  // Forward declaration

// https://en.cppreference.com/w/cpp/memory/weak_ptr
template <typename T>
class WeakPtr {
    template <typename Y>
    friend class WeakPtr;

    template <typename Y>
    friend class SharedPtr;
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    WeakPtr() = default;

    WeakPtr(const WeakPtr& other) : block_(other.block_), ptr_(other.ptr_) {
        if (block_) {
            block_->IncWeak();
        }
    }

    WeakPtr(WeakPtr&& other) : block_(other.block_), ptr_(other.ptr_) {
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    }

    template <typename Y>
    WeakPtr(const WeakPtr<Y>& other) : ptr_(other.ptr_), block_(other.block_) {
        if (block_) {
            block_->IncWeak();
        }
    }

    template <typename Y>
    WeakPtr(WeakPtr<Y>&& other) : block_(other.block_), ptr_(other.ptr_) {
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    }

    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    WeakPtr(const SharedPtr<T>& other) : block_(other.block_), ptr_(other.ptr_) {
        if (block_) {
            block_->IncWeak();
        }
    }

    template <typename Y>
    WeakPtr(const SharedPtr<Y>& other) : block_(other.block_), ptr_(other.ptr_) {
        if (block_) {
            block_->IncWeak();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    WeakPtr& operator=(const WeakPtr& other) {
        if (this == &other) {
            return *this;
        }
        Reset();
        block_ = other.block_;
        ptr_ = other.ptr_;
        if (block_) {
            block_->IncWeak();
        }
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) {
        if (this == &other) {
            return *this;
        }
        Reset();
        Swap(other);
        return *this;
    }

    template <typename Y>
    WeakPtr& operator=(const WeakPtr<Y>& other) {
        if (this == &other) {
            return *this;
        }
        Reset();
        block_ = other.block_;
        ptr_ = other.ptr_;
        if (block_) {
            block_->IncWeak();
        }
        return *this;
    }

    template <typename Y>
    WeakPtr& operator=(WeakPtr<Y>&& other) {
        if (this == &other) {
            return *this;
        }
        Reset();
        Swap(other);
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~WeakPtr() {
        if (block_) {
            block_->DecWeak();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (block_) {
            block_->DecWeak();
            block_ = nullptr;
        }
        ptr_ = nullptr;
    }

    void Swap(WeakPtr& other) {
        std::swap(block_, other.block_);
        std::swap(ptr_, other.ptr_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    size_t UseCount() const {
        return block_ ? block_->Strong() : 0;
    }

    bool Expired() const {
        return (UseCount() == 0);
    }

    SharedPtr<T> Lock() const {
        if (Expired()) {
            return SharedPtr<T>();
        }
        block_->IncStrong();
        return SharedPtr(ptr_, block_);
    }

private:
    T* ptr_ = nullptr;
    ControlBlockBase* block_ = nullptr;
};