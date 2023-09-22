#pragma once

#include <initializer_list>
#include <algorithm>
#include <deque>

class Deque {
private:
    static const size_t kBufferSize = 128;

    struct Buffer {
        int* data;
        bool have_data = false;

        Buffer() = default;

        void Alloc();

        void Dealloc();

        int& operator[](size_t index);

        int operator[](size_t index) const;
    };

public:
    Deque() = default;
    Deque(const Deque& other);
    Deque(Deque&& other);
    explicit Deque(size_t size);

    Deque(std::initializer_list<int> list);

    Deque& operator=(Deque other);

    void Swap(Deque& other);

    void PushBack(int value);

    void PopBack();

    void PushFront(int value);

    void PopFront();

    int& operator[](size_t ind);

    int operator[](size_t ind) const;

    size_t Size() const;

    void Clear();

    ~Deque();

private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    size_t amount_of_buffers_ = 0;
    std::pair<size_t, size_t> front_;
    std::pair<size_t, size_t> back_;
    Buffer* data_ = nullptr;

    void Reallocate();
};

