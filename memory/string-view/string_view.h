#pragma once

#include <string>
#include <cstring>

class StringView {
public:
    StringView(const std::string& str, size_t start = 0, size_t len = std::string::npos)
        : str_(str.data()), start_(start), len_(std::min(len, str.size() - start_)) {
    }

    explicit StringView(const char* str) : str_(str), start_(0), len_(strlen(str)) {
    }

    StringView(const char* str, size_t len)
        : str_(str), start_(0), len_(std::min(len, strlen(str))) {
    }

    char operator[](size_t index) const {
        return *(str_ + start_ + index);
    }

    size_t Size() const {
        return len_;
    }

private:
    const char* str_;
    size_t start_;
    size_t len_;
};
