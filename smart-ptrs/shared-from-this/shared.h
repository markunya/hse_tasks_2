#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t

// https://en.cppreference.com/w/cpp/memory/shared_ptr
template <typename T>
class EnableSharedFromThis;

class EnableSharedFromThisBase;

template <typename T>
class SharedPtr {
    template <typename Y>
    friend class SharedPtr;

    friend class WeakPtr<T>;

    template <typename Y>
    friend class WeakPtr;

    template <typename Y>
    friend class EnableSharedFromThis;

    public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr() = default;
    SharedPtr(std::nullptr_t) {
    }

    template <typename Y>
    explicit SharedPtr(Y* ptr) : ptr_(ptr) {
        block_ = new ControlBlock(ptr);
        if constexpr (std::is_convertible_v<Y, EnableSharedFromThisBase>) {
            InitWeakThis(ptr);
        }
    }

    SharedPtr(const SharedPtr& other) : ptr_(other.ptr_), block_(other.block_) {
        if (block_) {
            block_->IncStrong();
        }
        if constexpr (std::is_convertible_v<T, EnableSharedFromThisBase>) {
            InitWeakThis(ptr_);
        }
    }

    SharedPtr(SharedPtr&& other) : ptr_(other.ptr_), block_(other.block_) {
        other.ptr_ = nullptr;
        other.block_ = nullptr;
        if constexpr (std::is_convertible_v<T, EnableSharedFromThisBase>) {
            InitWeakThis(ptr_);
        }
    }

    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other) : ptr_(other.ptr_), block_(other.block_) {
        if (block_) {
            block_->IncStrong();
        }
        if constexpr (std::is_convertible_v<T, EnableSharedFromThisBase>) {
            InitWeakThis(ptr_);
        }
    }

    template <typename Y>
    SharedPtr(SharedPtr<Y>&& other) : ptr_(other.ptr_), block_(other.block_) {
        other.ptr_ = nullptr;
        other.block_ = nullptr;
        if constexpr (std::is_convertible_v<T, EnableSharedFromThisBase>) {
            InitWeakThis(ptr_);
        }
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) : block_(other.block_), ptr_(ptr) {
        if (block_) {
            block_->IncStrong();
        }
        if constexpr (std::is_convertible_v<T, EnableSharedFromThisBase>) {
            InitWeakThis(ptr_);
        }
    }

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other) {
        if (other.Expired()) {
            throw BadWeakPtr();
        }
        *this = std::move(other.Lock());
        if constexpr (std::is_convertible_v<T, EnableSharedFromThisBase>) {
            InitWeakThis(ptr_);
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    SharedPtr& operator=(const SharedPtr& other) {
        if (this == &other) {
            return *this;
        }
        Reset();
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (block_) {
            block_->IncStrong();
        }
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) {
        if (this == &other) {
            return *this;
        }
        Reset();
        ptr_ = other.ptr_;
        block_ = other.block_;
        other.ptr_ = nullptr;
        other.block_ = nullptr;
        return *this;
    }

    template <typename Y>
    SharedPtr& operator=(const SharedPtr<Y>& other) {
        Reset();
        ptr_ = other.ptr_;
        block_ = other.block_;
        if (block_) {
            block_->IncStrong();
        }
        return *this;
    }

    template <typename Y>
    SharedPtr& operator=(SharedPtr<Y>&& other) {
        Reset();
        ptr_ = other.ptr_;
        block_ = other.block_;
        other.ptr_ = nullptr;
        other.block_ = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() {
        if (block_) {
            block_->DecStrong();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (block_) {
            block_->DecStrong();
        }
        block_ = nullptr;
        ptr_ = nullptr;
    }

    void Reset(T* ptr) {
        Reset();
        block_ = new ControlBlock(ptr);
        ptr_ = ptr;
    }

    template <typename Y>
    void Reset(Y* ptr) {
        Reset();
        block_ = new ControlBlock(ptr);
        ptr_ = ptr;
    }

    void Swap(SharedPtr& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(block_, other.block_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return ptr_;
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

    size_t UseCount() const {
        return block_ ? block_->strong : 0;
    }

    explicit operator bool() const {
        return ptr_;
    }

    SharedPtr(T* ptr, ControlBlockBase* block) : ptr_(ptr), block_(block) {
        if constexpr (std::is_convertible_v<T, EnableSharedFromThisBase>) {
            InitWeakThis(ptr_);
        }
    }

private:
    T* ptr_ = nullptr;
    ControlBlockBase* block_ = nullptr;

    template <typename Y>
    void InitWeakThis(EnableSharedFromThis<Y>* e) {
        e->self_ = *this;
    }

};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) {
    return left.Get() == right.Get();
}

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    ObjectWithControlBlock<T>* obj_with_block =
        new ObjectWithControlBlock<T>(std::forward<Args>(args)...);
    return SharedPtr<T>(obj_with_block->GetPointer(), obj_with_block);
}



// Look for usage examples in tests

class EnableSharedFromThisBase {
public:
    EnableSharedFromThisBase() = default;
};

template <typename T>
class EnableSharedFromThis : public EnableSharedFromThisBase {
public:
    SharedPtr<T> SharedFromThis() {
        return self_.Lock();
    }

    SharedPtr<const T> SharedFromThis() const {
        return self_.Lock();
    }

    WeakPtr<T> WeakFromThis() noexcept {
        return self_;
    }

    WeakPtr<const T> WeakFromThis() const noexcept {
        return self_;
    }

protected:
    EnableSharedFromThis() = default;
    EnableSharedFromThis(const EnableSharedFromThis& other) {
    }
    EnableSharedFromThis& operator=(const EnableSharedFromThis& other) {
        return *this;
    }
    template <typename Y>
    EnableSharedFromThis& operator=(const EnableSharedFromThis<Y>& other) {
        return *this;
    }

private:
    template <typename U>
    friend class SharedPtr;

    WeakPtr<T> self_;
};