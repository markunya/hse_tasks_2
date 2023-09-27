#pragma once

#include <cstdint>
#include <string>
#include <vector>

class COWVector {
public:
    COWVector() : data_(new Data()) {}
    ~COWVector() {
        --data_->counter_of_refs;
        if (data_->counter_of_refs == 0) {
            delete data_;
        }
    }

    COWVector(const COWVector& other) : data_(other.data_) {
        ++data_->counter_of_refs;
    }

    COWVector& operator=(const COWVector& other) {
        if (this == &other) {
            return *this;
        }
        --data_->counter_of_refs;
        if (data_->counter_of_refs == 0) {
            delete data_;
        }
        data_ = other.data_;
        ++data_->counter_of_refs;
        return *this;
    }

    COWVector(COWVector&& other) {
        std::swap(data_, other.data_);
    }

    COWVector& operator=(COWVector&& other) {
        if (this == &other) {
            return *this;
        }
        --data_->counter_of_refs;
        if (data_->counter_of_refs == 0) {
            delete data_;
        }
        std::swap(data_, other.data_);
        return *this;
    }

    size_t Size() const {
        return data_->buffer.size();
    }

    void Resize(size_t size) {
        Reallocate();
        data_->buffer.resize(size);
    }

    const std::string& Get(size_t at) {
        return data_->buffer[at];
    }
    const std::string& Back() {
        return data_->buffer.back();
    }

    void PushBack(const std::string& value) {
        Reallocate();
        data_->buffer.push_back(value);
    }

    void Set(size_t at, const std::string& value) {
        Reallocate();
        data_->buffer[at] = value;
    }

private:
    struct Data {
        std::vector<std::string> buffer;
        size_t counter_of_refs;

        Data() : counter_of_refs(1) {}
    };

    Data* data_ = nullptr;

    void Reallocate() {
        if (data_->counter_of_refs > 1) {
            Data* new_data = new Data();
            new_data->buffer = data_->buffer;
            std::swap(data_, new_data);
        }
    }
};
