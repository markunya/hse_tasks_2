#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <cstring>

bool StartsWith(std::string_view string, std::string_view text);

bool EndsWith(std::string_view string, std::string_view text);

std::string_view StripPrefix(std::string_view string, std::string_view text);

std::string_view StripSuffix(std::string_view string, std::string_view text);

std::string_view ClippedSubstr(std::string_view s, size_t pos, size_t n = std::string_view::npos);

std::string_view StripAsciiWhitespace(std::string_view str);

std::vector<std::string_view> StrSplit(std::string_view text, std::string_view delim);

std::string ReadN(const std::string& filename, size_t n);

std::string AddSlash(std::string_view path);

std::string_view RemoveSlash(std::string_view path);

std::string_view Dirname(std::string_view path);

std::string_view Basename(std::string_view path);

std::string CollapseSlashes(std::string_view path);

std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter);

template <typename T>
size_t GetStringSize(const T& s) {
    if constexpr (std::is_integral_v<T>) {
        if (s == 0) {
            return 1;
        }
        T x = s;
        size_t result = 0;
        if (x < 0) {
            ++result;
        }
        while (x != 0) {
            ++result;
            x /= 10;
        }
        return result;
    } else if constexpr (std::is_convertible_v<T, const char*>) {
        return std::strlen(s);
    } else {
        return s.size();
    }
    return 0;
}

template <typename T>
void AddToString(std::string& res, const T& x) {
    if constexpr (std::is_integral_v<T>) {
        if (x == 0) {
            res += '0';
            return;
        }
        T t = x;
        if (x < 0) {
            res += '-';
        }
        auto b = res.end();
        while (t != 0) {
            res += '0' + ((t > 0) ? (t % 10) : -(t % 10));
            t /= 10;
        }
        std::reverse(b, res.end());
    } else {
        res += std::move(x);
    }
}

template <typename... Args>
std::string StrCat(const Args&... args) {
    std::string result;
    size_t sz = (0 + ... + GetStringSize(args));
    result.reserve(sz);
    (AddToString(result, args), ...);
    return result;
}