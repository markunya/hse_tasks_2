#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

using std::string;

std::vector<std::unique_ptr<string>> Duplicate(const std::vector<std::shared_ptr<string>>& items) {
    std::vector<std::unique_ptr<string>> result;
    for (const auto& item : items) {
        result.emplace_back(std::make_unique<string>(*item));
    }
    return result;
}

std::vector<std::shared_ptr<string>> DeDuplicate(
    const std::vector<std::unique_ptr<string>>& items) {
    std::unordered_map<std::string, std::shared_ptr<string>> m;
    std::vector<std::shared_ptr<string>> result;
    for (const auto& item : items) {
        if (!m.contains(*item)) {
            result.emplace_back(std::make_shared<string>(*item));
            m[*item] = result[result.size() - 1];
        } else {
            std::shared_ptr<string> s = m[*item];
            result.emplace_back(s);
        }
    }
    return result;
}
