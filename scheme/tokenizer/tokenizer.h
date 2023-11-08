#pragma once

#include <variant>
#include <optional>
#include <istream>
#include "error.h"

struct SymbolToken {
    std::string name;
    SymbolToken() = delete;
    SymbolToken(std::string str) : name(std::move(str)) {
    }

    bool operator==(const SymbolToken& other) const {
        return name == other.name;
    }
};

struct BooleanToken {
    bool value;
    BooleanToken() = delete;
    BooleanToken(bool x) : value(x) {
    }

    bool operator==(const BooleanToken& other) const {
        return value == other.value;
    }
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;
    ConstantToken() = delete;
    ConstantToken(int x) : value(x) {
    }

    bool operator==(const ConstantToken& other) const {
        return value == other.value;
    }
};

using Token =
    std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken, BooleanToken>;

class Tokenizer {
public:
    Tokenizer() = delete;

    Tokenizer(std::istream* in) : stream_(in) {
        Next();
    }

    bool IsEnd() const {
        return is_end_;
    }

    void Next() {
        SkipSpace();
        if (stream_->peek() == EOF) {
            is_end_ = true;
            return;
        }
        is_end_ = false;
        std::string str;
        str += static_cast<char>(stream_->get());
        if (str == "\'") {
            current_token_ = QuoteToken{};
            return;
        }
        if (str == ".") {
            current_token_ = DotToken{};
            return;
        }
        if (str == "(") {
            current_token_ = BracketToken::OPEN;
            return;
        }
        if (str == ")") {
            current_token_ = BracketToken::CLOSE;
            return;
        }
        if (str == "+" || str == "-" || std::isdigit(str.back())) {
            while (stream_->peek() != EOF && std::isdigit(static_cast<char>(stream_->peek()))) {
                str += stream_->get();
            }
            if (str == "+" || str == "-") {
                current_token_ = SymbolToken(str);
                return;
            }
            current_token_ = ConstantToken{std::stoi(str)};
            return;
        }
        auto is_symbol_beginning = [](char c) -> bool {
            return std::isalpha(c) || c == '<' || c == '=' || c == '>' || c == '*' || c == '/' ||
                   c == '#';
        };
        auto is_symbol_character = [](char c) -> bool {
            return std::isalnum(c) || c == '<' || c == '=' || c == '>' || c == '*' || c == '/' ||
                   c == '#' || c == '!' || c == '?' || c == '-';
        };
        if (is_symbol_beginning(str.back())) {
            while (stream_->peek() != EOF &&
                   is_symbol_character(static_cast<char>(stream_->peek()))) {
                str += static_cast<char>(stream_->get());
            }
            if (stream_->peek() != EOF && !std::isspace(static_cast<char>(stream_->peek()))) {
                throw SyntaxError(str);
            }
            current_token_ = SymbolToken{str};
            return;
        }
        throw SyntaxError("str");
    }

    Token GetToken() {
        return current_token_;
    }

private:
    void SkipSpace() {
        while (stream_->peek() != EOF && std::isspace(stream_->peek())) {
            stream_->get();
        }
    }
    bool is_end_ = false;
    Token current_token_ = QuoteToken{};
    std::istream* stream_;
};