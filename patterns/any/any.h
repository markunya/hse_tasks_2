#pragma once

#include <typeinfo>
#include <algorithm>

class Any {
public:
    Any() = default;

    template <class T>
    Any(const T& value) : ptr_(new Inner(value)) {
    }

    template <class T>
    Any& operator=(const T& value) {
        Any a(value);
        Swap(a);
        return *this;
    }

    Any(const Any& rhs) : ptr_(rhs.ptr_->Clone()) {
    }
    Any& operator=(Any rhs) {
        Swap(rhs);
        return *this;
    }
    ~Any() {
        Clear();
    }

    bool Empty() const {
        return !ptr_;
    }

    void Clear() {
        delete ptr_;
        ptr_ = nullptr;
    }

    void Swap(Any& rhs) {
        std::swap(rhs.ptr_, ptr_);
    }

    template <class T>
    const T& GetValue() const {
        if (typeid(T) != ptr_->Type()) {
            throw std::bad_cast();
        }
        auto x = dynamic_cast<Inner<T>*>(ptr_);
        return x->value;
    }

private:
    struct InnerBase {
        virtual ~InnerBase() = default;
        virtual InnerBase* Clone() const = 0;
        virtual const std::type_info& Type() const = 0;
    };

    template <typename T>
    struct Inner : InnerBase {
        T value;
        virtual InnerBase* Clone() const {
            return (new Inner(value));
        }
        virtual const std::type_info& Type() const {
            return typeid(T);
        }
        Inner(T x) : value(std::move(x)) {
        }
    };

    InnerBase* ptr_ = nullptr;
};
