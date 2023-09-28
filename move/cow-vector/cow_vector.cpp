#include "cow_vector.h"

COWVector::COWVector() : data_(new Data()) {
}

COWVector::~COWVector() {
    --data_->counter_of_refs;
    if (data_->counter_of_refs == 0) {
        delete data_;
    }
}

COWVector::COWVector(const COWVector& other) : data_(other.data_) {
    ++data_->counter_of_refs;
}

COWVector& COWVector::operator=(const COWVector& other) {
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

COWVector::COWVector(COWVector&& other) {
    data_ = other.data_;
    ++data_->counter_of_refs;
}

COWVector& COWVector::operator=(COWVector&& other) {
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

size_t COWVector::Size() const {
    return data_->buffer.size();
}

void COWVector::Resize(size_t size) {
    Reallocate();
    data_->buffer.resize(size);
}

const std::string& COWVector::Get(size_t at) {
    return data_->buffer[at];
}

const std::string& COWVector::Back() {
    return data_->buffer.back();
}

void COWVector::PushBack(const std::string& value) {
    Reallocate();
    data_->buffer.push_back(value);
}

void COWVector::Set(size_t at, const std::string& value) {
    Reallocate();
    data_->buffer[at] = value;
}

void COWVector::Reallocate() {
    if (data_->counter_of_refs > 1) {
        --data_->counter_of_refs;
        Data* new_data = new Data();
        new_data->buffer = data_->buffer;
        data_ = new_data;
    }
}

COWVector::Data::Data() : counter_of_refs(1) {
}
