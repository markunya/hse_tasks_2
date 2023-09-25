#pragma once

#include <initializer_list>
#include <algorithm>
#include <deque>

class Deque {
private:
    static const size_t kBufferSize = 128;

    struct Buffer {
        int* data = nullptr;
        bool have_data = false;

        Buffer() = default;

        void Alloc() {
            if (have_data) {
                return;
            }
            data = new int[kBufferSize];
            for (size_t i = 0; i < kBufferSize; ++i) {
                data[i] = 0;
            }
            have_data = true;
        }


        void Dealloc() {
            if (!have_data) {
                return;
            }
            delete[] data;
            have_data = false;
        }

        int& operator[](size_t index) {
            return data[index];
        }

        int operator[](size_t index) const {
            return data[index];
        }
    };

public:
    Deque() = default;

    Deque(const Deque& other)
        : capacity_(other.capacity_), front_(std::make_pair(0, 0)), back_(std::make_pair(0, 0)), data_(new Buffer[other.capacity_]) {
        for (size_t i = 0; i < other.size_; ++i) {
            PushBack(other[i]);
        }
    }

    Deque(Deque&& other) {
        Swap(other);
    }

    explicit Deque(size_t size) {
        capacity_ = ((size + kBufferSize - 1) / kBufferSize);
        data_ = new Buffer[capacity_];
        size_ = 0;
        front_ = std::make_pair(0, 0);
        back_ = std::make_pair(0, 0);
        for (size_t i = 0; i < size; ++i) {
            PushBack(0);
        }
    }

    Deque(std::initializer_list<int> list) {
        capacity_ = ((list.size() + kBufferSize - 1) / kBufferSize);
        data_ = new Buffer[capacity_];
        front_ = std::make_pair(0, 0);
        back_ = std::make_pair(0, 0);
        for (auto i : list) {
            PushBack(i);
        }
    }

    Deque& operator=(Deque other) {
        Swap(other);
        return *this;
    }

    void Swap(Deque& other) {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(amount_of_buffers_, other.amount_of_buffers_);
        std::swap(front_, other.front_);
        std::swap(back_, other.back_);
        std::swap(data_, other.data_);
    }

    void PushBack(int value) {
        if (back_.second == 0 && amount_of_buffers_ == capacity_) {
            Reallocate();
        }
        if (!data_[back_.first].have_data) {
            data_[back_.first].Alloc();
            ++amount_of_buffers_;
        }
        data_[back_.first][back_.second] = value;
        if (back_.second == kBufferSize - 1) {
            back_.second = 0;
            if (back_.first == capacity_ - 1) {
                back_.first = 0;
            } else {
                ++back_.first;
            }
        } else {
            ++back_.second;
        }
        ++size_;
    }


    void PopBack() {
        if (back_.second == 1) {
            data_[back_.first].Dealloc();
            --amount_of_buffers_;
        }
        if (back_.second == 0) {
            back_.second = kBufferSize - 1;
            if (back_.first == 0) {
                back_.first = capacity_ - 1;
            } else {
                --back_.first;
            }
        } else {
            --back_.second;
        }
        --size_;
    }

    void PushFront(int value) {
        if (front_.second == 0) {
            if (amount_of_buffers_ == capacity_) {
                Reallocate();
            }
            front_.second = kBufferSize - 1;
            if (front_.first == 0) {
                front_.first = capacity_ - 1;
            } else {
                --front_.first;
            }
            data_[front_.first].Alloc();
            ++amount_of_buffers_;
        } else {
            --front_.second;
        }
        data_[front_.first][front_.second] = value;
        ++size_;
    }

    void PopFront() {
        if (front_.second == kBufferSize - 1) {
            data_[front_.first].Dealloc();
            --amount_of_buffers_;
            front_.second = 0;
            if (front_.first == capacity_ - 1) {
                front_.first = 0;
            } else {
                ++front_.first;
            }
        } else {
            ++front_.second;
        }
        --size_;
    }


    int& operator[](size_t ind) {
        if (ind <= kBufferSize - front_.second - 1) {
            return data_[front_.first][front_.second + ind];
        }
        ind -= kBufferSize - front_.second;
        return data_[(front_.first + 1 + ind / 128) % capacity_][ind % 128];
    }


    int operator[](size_t ind) const {
        if (ind <= kBufferSize - front_.second - 1) {
            return data_[front_.first][front_.second + ind];
        }
        ind -= kBufferSize - front_.second;
        return data_[(front_.first + 1 + ind / 128) % capacity_][ind % 128];
    }


    size_t Size() const {
        return size_;
    }

    void Clear() {
        for (size_t i = 0; i < capacity_; ++i) {
            data_[i].Dealloc();
        }
        amount_of_buffers_ = 0;
        size_ = 0;
        front_ = std::make_pair(0, 0);
        back_ = std::make_pair(0, 0);
    }


    ~Deque() {
        Clear();
        delete[] data_;
    }

private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    size_t amount_of_buffers_ = 0;
    std::pair<size_t, size_t> front_;
    std::pair<size_t, size_t> back_;
    Buffer* data_ = nullptr;

    void Reallocate() {
        if (capacity_ == 0) {
            capacity_ = 1;
            data_ = new Buffer[1];
            return;
        }
        size_t new_capacity = capacity_ * 2;
        Buffer* new_data = new Buffer[new_capacity];
        size_t current_index = 0;
        std::pair<size_t, size_t> new_front = std::make_pair(0, front_.second);
        for (; current_index + front_.first < capacity_; ++current_index) {
            new_data[current_index] = data_[front_.first + current_index];
        }
        for (size_t i = 0; i < back_.first; ++i) {
            new_data[current_index + i] = data_[i];
        }
        std::pair<size_t, size_t> new_back = std::make_pair(current_index + back_.first, back_.second);
        current_index += back_.first;
        if (back_.second > 0 && front_.first != back_.first) {
            new_data[current_index] = data_[back_.first];
        }
        capacity_ = new_capacity;
        front_ = new_front;
        back_ = new_back;
        delete[] data_;
        data_ = new_data;
    }
};