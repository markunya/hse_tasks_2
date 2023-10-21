#pragma once

#include <optional>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include "utf8.h"

using std::optional;
namespace fs = std::filesystem;

struct GrepOptions {
    optional<size_t> look_ahead_length;
    size_t max_matches_per_line;

    GrepOptions() {
        max_matches_per_line = 10;
    }

    GrepOptions(size_t look_ahead_length) : GrepOptions() {
        this->look_ahead_length = look_ahead_length;
    }

    GrepOptions(optional<size_t> look_ahead_length, size_t max_matches_per_line) {
        this->look_ahead_length = look_ahead_length;
        this->max_matches_per_line = max_matches_per_line;
    }
};

std::string GetSubstr(const std::string& str, size_t pos, size_t len) {
    std::vector<uint16_t> utf16line;
    utf8::utf8to16(str.begin(), str.end(), back_inserter(utf16line));
    std::string result;
    size_t sz = utf16line.size();
    size_t end = (pos + len < sz) ? (pos + len) : sz;
    utf8::utf16to8(utf16line.begin() + pos, utf16line.begin() + end, back_inserter(result));
    return result;
}

struct MatchObj {
    std::string path;
    size_t line;
    size_t column;
    std::optional<std::string> context;
    MatchObj() = default;
};

template <class Visitor>
void Grep(const std::string& path, const std::string& pattern, Visitor visitor,
          const GrepOptions& options) noexcept {
    if (!fs::exists(path)) {
        return;
    }
    if (fs::is_regular_file(path) && !fs::is_symlink(path)) {
        try {
            std::vector<MatchObj> matches;
            std::ifstream input_stream(path);
            std::string line;
            size_t i = 0;
            while (getline(input_stream, line)) {
                auto it = line.begin();
                size_t counter = 0;
                while ((it = std::search(it, line.end(),
                                         std::boyer_moore_searcher(pattern.begin(),
                                                                   pattern.end()))) != line.end() &&
                       counter < options.max_matches_per_line) {
                    ++counter;
                    MatchObj match;
                    size_t pos = utf8::distance(line.begin(), it);
                    if (options.look_ahead_length.has_value()) {
                        match.context =
                            GetSubstr(line, pos + utf8::distance(pattern.begin(), pattern.end()),
                                      options.look_ahead_length.value());
                    }
                    match.line = i + 1;
                    match.column = pos + 1;
                    match.path = path;
                    matches.emplace_back(match);
                    ++it;
                }
                ++i;
            }
            for (auto match : matches) {
                visitor.OnMatch(match.path, match.line, match.column, match.context);
            }
        } catch (const std::exception& e) {
            visitor.OnError(e.what());
        }
    } else if (fs::is_directory(path)) {
        for (auto& file : fs::recursive_directory_iterator(path)) {
            if (fs::is_regular_file(file.path()) && !fs::is_symlink(file.path())) {
                try {
                    std::vector<MatchObj> matches;
                    std::ifstream input_stream(file.path().string());
                    std::string line;
                    size_t i = 0;
                    while (getline(input_stream, line)) {
                        auto it = line.begin();
                        size_t counter = 0;
                        while ((it = std::search(
                                    it, line.end(),
                                    std::boyer_moore_searcher(pattern.begin(), pattern.end()))) !=
                                   line.end() &&
                               counter < options.max_matches_per_line) {
                            ++counter;
                            MatchObj match;
                            size_t pos = utf8::distance(line.begin(), it);
                            if (options.look_ahead_length.has_value()) {
                                match.context = GetSubstr(
                                    line, pos + utf8::distance(pattern.begin(), pattern.end()),
                                    options.look_ahead_length.value());
                            }
                            match.line = i + 1;
                            match.column = pos + 1;
                            match.path = file.path().string();
                            matches.emplace_back(match);
                            ++it;
                        }
                        ++i;
                    }
                    for (auto match : matches) {
                        visitor.OnMatch(match.path, match.line, match.column, match.context);
                    }
                } catch (const std::exception& e) {
                    visitor.OnError(e.what());
                }
            }
        }
    }
}
