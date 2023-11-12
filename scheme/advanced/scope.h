#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "object.h"

class Scope : public std::enable_shared_from_this<Scope> {
public:
    Scope(std::shared_ptr<Scope> ptr = nullptr);
    Scope(std::initializer_list<std::pair<std::string, std::shared_ptr<Object>>> list,
          Scope* ptr = nullptr);

    bool Contains(const std::string& key);

    std::shared_ptr<Object> Get(const std::string& key);

    void Define(const std::string& key, std::shared_ptr<Object> value);

    void Set(const std::string& key, std::shared_ptr<Object> value);

    Scope* Up();

    std::shared_ptr<Scope> GetPtr() {
        return shared_from_this();
    }



    void Clear();

private:
    std::shared_ptr<Scope> next_ = nullptr;
    std::unique_ptr<std::unordered_map<std::string, std::shared_ptr<Object>>> data_ =
        std::make_unique<std::unordered_map<std::string, std::shared_ptr<Object>>>();
};
