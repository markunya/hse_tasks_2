#pragma once

#include <cstdint>
#include <string>
#include <vector>

class COWVector {
public:
    COWVector();
    ~COWVector();

    COWVector(const COWVector& other);

    COWVector& operator=(const COWVector& other);

    COWVector(COWVector&& other);

    COWVector& operator=(COWVector&& other);

    size_t Size() const;

    void Resize(size_t size);

    const std::string& Get(size_t at);

    const std::string& Back();

    void PushBack(const std::string& value);

    void Set(size_t at, const std::string& value);

private:
    struct Data {
        std::vector<std::string> buffer;
        size_t counter_of_refs;

        Data();
    };

    Data* data_ = nullptr;

    void Reallocate();
};
