#include "string_operations.h"

#include <string>
#include <unistd.h>
#include <fcntl.h>

bool StartsWith(std::string_view string, std::string_view text) {
    if (text.size() > string.size()) {
        return false;
    }
    for (size_t i = 0; i < text.size(); ++i) {
        if (string[i] != text[i]) {
            return false;
        }
    }
    return true;
}

bool EndsWith(std::string_view string, std::string_view text) {
    if (text.size() > string.size()) {
        return false;
    }
    for (size_t i = 0; i < text.size(); ++i) {
        if (string[string.size() - 1 - i] != text[text.size() - 1 - i]) {
            return false;
        }
    }
    return true;
}

std::string_view StripPrefix(std::string_view string, std::string_view text) {
    if (!StartsWith(string, text)) {
        return string;
    }
    string.remove_prefix(text.size());
    return string;
}

std::string_view StripSuffix(std::string_view string, std::string_view text) {
    if (!EndsWith(string, text)) {
        return string;
    }
    string.remove_suffix(text.size());
    return string;
}

std::string_view ClippedSubstr(std::string_view str, size_t pos, size_t n) {
    str.remove_prefix(pos);
    if (str.size() >= n) {
        str.remove_suffix(str.size() - n);
    }
    return str;
}

std::string_view StripAsciiWhitespace(std::string_view str) {
    while (!str.empty() && std::isspace(str.front())) {
        str.remove_prefix(1);
    }
    while (!str.empty() && std::isspace(str.back())) {
        str.remove_suffix(1);
    }
    return str;
}

std::vector<std::string_view> StrSplit(std::string_view text, std::string_view delim) {
    size_t counter = 0;
    for (size_t i = 0; i < text.size();) {
        for (size_t j = 0; j < delim.size();) {
            if (text[i] == delim[j]) {
                ++j;
                ++i;
                if (j == delim.size()) {
                    ++counter;
                    break;
                }
            } else {
                ++i;
                break;
            }
        }
    }
    size_t start = 0;
    size_t l = 0;
    std::vector<std::string_view> result(counter + 1);
    for (size_t i = 0; i < text.size();) {
        for (size_t j = 0; j < delim.size();) {
            if (text[i] == delim[j]) {
                ++j;
                ++i;
                if (j == delim.size()) {
                    result[l] = ClippedSubstr(text, start, i - j - start);
                    ++l;
                    start = i;
                    break;
                }
            } else {
                ++i;
                break;
            }
        }
    }
    result[l] = ClippedSubstr(text, start);
    return result;
}

std::string ReadN(const std::string& filename, size_t n) {
    std::string result;
    int file = open(filename.c_str(), O_RDONLY);
    if (file == -1) {
        return result;
    }
    char buffer[1024];
    read(file, buffer, n);
    result = std::string(buffer);
    close(file);
    return result;
}

std::string AddSlash(std::string_view path) {
    if (!path.empty() && EndsWith(path, "/")) {
        return std::string(path);
    }
    std::string result(path.size() + 1, '/');
    for (size_t i = 0; i < path.size(); ++i) {
        result[i] = path[i];
    }
    return result;
}

std::string_view RemoveSlash(std::string_view path) {
    if (path.empty() || !EndsWith(path, "/") || path.size() == 1) {
        return path;
    }
    path.remove_suffix(1);
    return path;
}

std::string_view Dirname(std::string_view path) {
    path.remove_suffix(path.size() - path.rfind('/') - 1);
    if (path.size() == 1) {
        return path;
    }
    path.remove_suffix(1);
    return path;
}
std::string_view Basename(std::string_view path) {
    path.remove_prefix(path.rfind('/') + 1);
    return path;
}

std::string CollapseSlashes(std::string_view path) {
    if (path.empty()) {
        return std::string();
    }
    size_t length = 1;
    for (size_t i = 0; i < path.size(); ++i) {
        if (i + 1 == path.size() || (path[i] == '/' && path[i + 1] == '/')) {
            continue;
        }
        ++length;
    }
    std::string result(length, '@');
    size_t l = 0;
    for (size_t i = 0; i < path.size(); ++i) {
        if (i + 1 == path.size() || (path[i] == '/' && path[i + 1] == '/')) {
            continue;
        }
        result[l] = path[i];
        ++l;
    }
    result[l] = path.back();
    return result;
}

std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter) {
    if (strings.empty()) {
        return std::string();
    }
    size_t length = 0;
    for (auto& i : strings) {
        length += i.size();
    }
    length += delimiter.size() * (strings.size() - 1);
    std::string result(length, '@');
    size_t l = 0;
    for (size_t i = 0; i < strings.size(); ++i) {
        for (size_t j = 0; j < strings[i].size(); ++j) {
            result[l] = strings[i][j];
            ++l;
        }
        if (i == strings.size() - 1) {
            break;
        }
        for (size_t j = 0; j < delimiter.size(); ++j) {
            result[l] = delimiter[j];
            ++l;
        }
    }
    return result;
}