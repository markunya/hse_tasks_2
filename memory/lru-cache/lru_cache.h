#pragma once

#include <string>
#include <unordered_map>

class LruCache {
public:
    LruCache(size_t max_size);

    void Set(const std::string& key, const std::string& value);

    bool Get(const std::string& key, std::string* value);

    ~LruCache();

private:
    struct Node {
        std::string val;
        std::string key;
        Node* next;
        Node* prev;
        Node(std::string  val, std::string  key);
    };

    size_t capacity_;
    size_t size_;
    std::unordered_map<std::string, Node*> m_;
    Node* start_;
    Node* finish_;
};
