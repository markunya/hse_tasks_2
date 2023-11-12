#include <tokenizer.h>

SymbolToken::SymbolToken(std::string str) : name(std::move(str)) {
}

bool SymbolToken::operator==(const SymbolToken &other) const {
    return name == other.name;
}

bool QuoteToken::operator==(const QuoteToken &) const {
    return true;
}

bool DotToken::operator==(const DotToken &) const {
    return true;
}

ConstantToken::ConstantToken(int x) : value(x) {
}

bool ConstantToken::operator==(const ConstantToken &other) const {
    return value == other.value;
}

BooleanToken::BooleanToken(bool x) : value(x) {
}

bool BooleanToken::operator==(const BooleanToken &other) const {
    return value && other.value;
}

Tokenizer::Tokenizer(std::istream *in) : stream_(in) {
    Next();
}

bool Tokenizer::IsEnd() const {
    return is_end_;
}

void Tokenizer::Next() {
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
        if (str.back() == '#' && (static_cast<char>(stream_->peek()) == 't' ||
                                  static_cast<char>(stream_->peek()) == 'f')) {
            str += static_cast<char>(stream_->get());
            current_token_ = BooleanToken(str == "#t");
            return;
        }
        while (stream_->peek() != EOF && is_symbol_character(static_cast<char>(stream_->peek()))) {
            str += static_cast<char>(stream_->get());
        }
        current_token_ = SymbolToken{str};
        return;
    }
    throw SyntaxError("str");
}

Token Tokenizer::GetToken() {
    return current_token_;
}

void Tokenizer::SkipSpace() {
    while (stream_->peek() != EOF && std::isspace(stream_->peek())) {
        stream_->get();
    }
}
