#pragma once

#include <cstddef>  // for std::nullptr_t
#include <utility>  // for std::exchange / std::swap

class SimpleCounter {
public:
    size_t IncRef() {
        return ++count_;
    }

    size_t DecRef() {
        return --count_;
    }

    size_t RefCount() const {
        return count_;
    }

private:
    size_t count_ = 0;
};

struct DefaultDelete {
    template <typename T>
    static void Destroy(T* object) {
        delete object;
    }
};

template <typename Derived, typename Counter, typename Deleter>
class RefCounted {
public:
    // Increase reference counter.
    void IncRef() {
        counter_.IncRef();
    }

    // Decrease reference counter.
    // Destroy object using Deleter when the last instance dies.
    void DecRef() {
        if (counter_.DecRef() == 0) {
            Deleter del;
            del.Destroy(static_cast<Derived*>(this));
        }
    }

    // Get current counter value (the number of strong references).
    size_t RefCount() const {
        return counter_.RefCount();
    }

private:
    Counter counter_;
};

template <typename Derived, typename D = DefaultDelete>
using SimpleRefCounted = RefCounted<Derived, SimpleCounter, D>;

template <typename T>
class IntrusivePtr {
    template <typename Y>
    friend class IntrusivePtr;

public:
    // Constructors
    IntrusivePtr() = default;
    IntrusivePtr(std::nullptr_t) {
    }
    IntrusivePtr(T* ptr) {
        obj_ = ptr;
        obj_->IncRef();
    }

    template <typename Y>
    IntrusivePtr(const IntrusivePtr<Y>& other) {
        if (!other) {
            return;
        }
        obj_ = other.obj_;
        obj_->IncRef();
    }

    template <typename Y>
    IntrusivePtr(IntrusivePtr<Y>&& other) {
        obj_ = other.obj_;
        other.obj_ = nullptr;
    }

    IntrusivePtr(const IntrusivePtr& other) {
        if (!other) {
            return;
        }
        obj_ = other.obj_;
        obj_->IncRef();
    }

    IntrusivePtr(IntrusivePtr&& other) {
        Swap(other);
    }

    // `operator=`-s
    IntrusivePtr& operator=(const IntrusivePtr& other) {
        if (this == &other) {
            return *this;
        }
        Reset();
        if (other) {
            obj_ = other.obj_;
            obj_->IncRef();
        }
        return *this;
    }

    IntrusivePtr& operator=(IntrusivePtr&& other) {
        Swap(other);
        return *this;
    }

    // Destructor
    ~IntrusivePtr() {
        if (*this) {
            obj_->DecRef();
        }
    }

    // Modifiers
    void Reset() {
        if (*this) {
            obj_->DecRef();
            obj_ = nullptr;
        }
    }
    void Reset(T* ptr) {
        if (*this) {
            obj_->DecRef();
        }
        obj_ = ptr;
        obj_->IncRef();
    }

    void Swap(IntrusivePtr& other) {
        std::swap(obj_, other.obj_);
    }

    // Observers
    T* Get() const {
        return obj_;
    }

    T& operator*() const {
        return *obj_;
    }

    T* operator->() const {
        return obj_;
    }

    size_t UseCount() const {
        if (!obj_) {
            return 0;
        }
        return obj_->RefCount();
    }

    explicit operator bool() const {
        return obj_;
    }

private:
    T* obj_ = nullptr;
};

template <typename T, typename... Args>
IntrusivePtr<T> MakeIntrusive(Args&&... args) {
    return IntrusivePtr<T>(new T(args...));
}
