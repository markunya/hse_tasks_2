#pragma once

#include "compressed_pair.h"

#include <cstddef>

template <typename T>
struct Slug {
    Slug() = default;
    template <typename U>
    Slug(Slug<U>&&) {
    }
    template <typename U>
    Slug& operator=(Slug<U>&&) {
    }
    void operator()(T* ptr) {
        delete ptr;
    }
};

template <typename T>
struct Slug<T[]> {
    Slug() = default;
    template <typename U>
    Slug(Slug<U>&&) {
    }
    template <typename U>
    Slug& operator=(Slug<U>&&) {
    }
    void operator()(T* ptr) {
        delete[] ptr;
    }
};

// Primary template
template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
public:
    UniquePtr() = default;

    template <typename U>
    explicit UniquePtr(U* ptr) : data_(ptr, Deleter()) {
    }

    template <typename U>
    UniquePtr(T* ptr, const U& deleter) : data_(ptr, deleter) {
    }

    template <typename U>
    UniquePtr(T* ptr, U&& deleter) : data_(ptr, std::move(deleter)) {
    }

    UniquePtr(const UniquePtr& other) = delete;

    template <typename U>
    UniquePtr(U&& other) noexcept : data_(other.Release(), std::move(other.GetDeleter())) {
    }

    UniquePtr& operator=(const UniquePtr& other) = delete;

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        Swap(other);
        other.Reset();
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        Reset();
        return *this;
    }

    ~UniquePtr() {
        Reset();
    }

    T* Release() {
        T* x = data_.GetFirst();
        data_.GetFirst() = nullptr;
        return x;
    }

    void Reset(T* ptr = nullptr) {
        std::swap(data_.GetFirst(), ptr);
        (data_.GetSecond())(ptr);
    }

    void Swap(UniquePtr& other) {
        std::swap(data_, other.data_);
    }

    T* Get() const {
        return data_.GetFirst();
    }

    Deleter& GetDeleter() {
        return data_.GetSecond();
    }

    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }

    explicit operator bool() const {
        return data_.GetFirst() != nullptr;
    }

    auto operator*() const {
        return *data_.GetFirst();
    }

    T* operator->() const {
        return data_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> data_;
};

// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    UniquePtr() = default;

    template <typename U>
    explicit UniquePtr(U* ptr) : data_(ptr, Deleter()) {
    }

    template <typename U>
    UniquePtr(T* ptr, const U& deleter) : data_(ptr, deleter) {
    }

    template <typename U>
    UniquePtr(T* ptr, U&& deleter) : data_(ptr, std::move(deleter)) {
    }

    UniquePtr(const UniquePtr& other) = delete;

    template <typename U>
    UniquePtr(U&& other) noexcept : data_(other.Release(), std::move(other.GetDeleter())) {
    }

    UniquePtr& operator=(const UniquePtr& other) = delete;

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        Swap(other);
        other.Reset();
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        Reset();
        return *this;
    }

    ~UniquePtr() {
        Reset();
    }

    T* Release() {
        T* x = data_.GetFirst();
        data_.GetFirst() = nullptr;
        return x;
    }

    void Reset(T* ptr = nullptr) {
        std::swap(data_.GetFirst(), ptr);
        (data_.GetSecond())(ptr);
    }

    void Swap(UniquePtr& other) {
        std::swap(data_, other.data_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return data_.GetFirst();
    }

    Deleter& GetDeleter() {
        return data_.GetSecond();
    }

    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }

    explicit operator bool() const {
        return data_.GetFirst() != nullptr;
    }

    auto operator*() const {
        return *data_.GetFirst();
    }

    T* operator->() const {
        return data_.GetFirst();
    }

    T& operator[](size_t index) {
        return data_.GetFirst()[index];
    }

    const T& operator[](size_t index) const {
        return data_.GetFirst()[index];
    }

private:
    CompressedPair<T*, Deleter> data_;
};
