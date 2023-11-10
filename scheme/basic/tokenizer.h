#pragma once

#include <variant>
#include <optional>
#include <istream>
#include "error.h"

struct SymbolToken {
    std::string name;
    SymbolToken() = delete;
    SymbolToken(std::string str);

    bool operator==(const SymbolToken& other) const;
};

struct BooleanToken {
    bool value;
    BooleanToken() = delete;
    BooleanToken(bool x);

    bool operator==(const BooleanToken& other) const;
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const;
};

struct DotToken {
    bool operator==(const DotToken&) const;
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;
    ConstantToken() = delete;
    ConstantToken(int x);

    bool operator==(const ConstantToken& other) const;
};

using Token =
    std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken, BooleanToken>;

class Tokenizer {
public:
    Tokenizer() = delete;

    Tokenizer(std::istream* in);

    bool IsEnd() const;

    void Next();

    Token GetToken();

private:
    void SkipSpace();

    bool is_end_ = false;
    Token current_token_ = QuoteToken{};
    std::istream* stream_;
};