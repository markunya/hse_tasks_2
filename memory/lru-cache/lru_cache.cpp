#include "lru_cache.h"

LruCache::LruCache(size_t max_size)
      : capacity_(max_size), size_(0), start_(new Node("", "")), finish_(new Node("", "")) {
    start_->next = finish_;
    finish_->prev = start_;
}
void LruCache::Set(const std::string& key, const std::string& value) {
    if (m_.count(key)) {
        Node* x = m_[key];
        x->prev->next = x->next;
        x->next->prev = x->prev;
        x->next = finish_;
        x->prev = finish_->prev;
        x->prev->next = x;
        finish_->prev = x;
        x->val = value;
        return;
    }
    if (size_ < capacity_) {
        Node* t = new Node(value, key);
        t->next = finish_;
        t->prev = finish_->prev;
        t->prev->next = t;
        finish_->prev = t;
        ++size_;
        m_[key] = t;
        return;
    }
    Node* x = start_->next;
    x->prev->next = x->next;
    x->next->prev = x->prev;
    x->next = finish_;
    x->prev = finish_->prev;
    x->prev->next = x;
    finish_->prev = x;
    m_.erase(x->key);
    x->key = key;
    x->val = value;
    m_[key] = x;
}

bool LruCache::Get(const std::string& key, std::string* value) {
    if (m_.count(key)) {
        Node* x = m_[key];
        x->prev->next = x->next;
        x->next->prev = x->prev;
        x->next = finish_;
        x->prev = finish_->prev;
        x->prev->next = x;
        finish_->prev = x;
        *value = x->val;
        return true;
    }
    return false;
}
LruCache::~LruCache() {
    Node* current = start_->next;
    while (current != nullptr) {
        delete current->prev;
        current = current->next;
    }
    delete finish_;
}

LruCache::Node::Node(std::string  val, std::string  key)
      : val(std::move(val)), key(std::move(key)), next(nullptr), prev(nullptr) {
}
