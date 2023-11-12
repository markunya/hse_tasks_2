#include "scope.h"

Scope::Scope(std::shared_ptr<Scope> ptr) : next_(ptr) {
}

Scope::Scope(std::initializer_list<std::pair<std::string, std::shared_ptr<Object>>> list,
             Scope* ptr)
    : next_(ptr),
      data_(std::make_unique<std::unordered_map<std::string, std::shared_ptr<Object>>>(
          list.begin(), list.end())) {
}

std::shared_ptr<Object> Scope::Get(const std::string& key) {
    if (!Contains(key)) {
        if (!Up()) {
            throw NameError("Name not found: " + key);
        }
        return Up()->Get(key);
    }
    return data_->operator[](key);
}

void Scope::Define(const std::string& key, std::shared_ptr<Object> value) {
    data_->operator[](key) = value;
}

void Scope::Set(const std::string& key, std::shared_ptr<Object> value) {
    if (!Contains(key)) {
        if (!Up()) {
            throw NameError("Name not found: " + key);
        }
        Up()->Set(key, value);
        return;
    }
    data_->operator[](key) = value;
}

Scope* Scope::Up() {
    return next_.get();
}

bool Scope::Contains(const std::string& key) {
    return data_->contains(key);
}

void Scope::Clear() {
    data_->clear();
}
