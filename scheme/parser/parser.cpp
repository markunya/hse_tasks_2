#include <parser.h>

std::shared_ptr<Object> ReadDefault(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("");
    }
    Token token = tokenizer->GetToken();
    if (std::holds_alternative<ConstantToken>(token)) {
        tokenizer->Next();
        return std::shared_ptr<Object>(new Number(std::get<ConstantToken>(token).value));
    }
    if (std::holds_alternative<SymbolToken>(token)) {
        tokenizer->Next();
        return std::shared_ptr<Object>(new Symbol(std::get<SymbolToken>(token).name));
    }
    if (std::holds_alternative<BooleanToken>(token)) {
        tokenizer->Next();
        return std::shared_ptr<Object>(new Boolean(std::get<BooleanToken>(token).value));
    }
    if (std::holds_alternative<DotToken>(token)) {
        tokenizer->Next();
        throw SyntaxError("dot");
    }
    if (std::holds_alternative<BracketToken>(token)) {
        if (std::get<BracketToken>(token) == BracketToken::OPEN) {
            tokenizer->Next();
            return ReadList(tokenizer);
        }
        tokenizer->Next();
        throw SyntaxError("troubles with brackets");
    }
    throw SyntaxError("");
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    try {
        return std::shared_ptr<Object>(new Cell(ReadDefault(tokenizer), ReadList(tokenizer)));
    } catch (const SyntaxError& e) {
        if (strcmp(e.what(), "troubles with brackets") == 0) {
            return nullptr;
        }
        if (strcmp(e.what(), "dot") == 0) {
            std::shared_ptr<Object> res = nullptr;
            try {
                res = ReadDefault(tokenizer);
            } catch (const SyntaxError& e) {
                if (strcmp(e.what(), "troubles with brackets") == 0 || strcmp(e.what(), "dot") == 0) {
                    throw SyntaxError("");
                }
            }
            if (tokenizer->IsEnd()) {
                throw SyntaxError("");
            }
            tokenizer->Next();
            return res;
        }
        throw;
    }
}

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("no code");
    }
    std::shared_ptr<Object> result = ReadDefault(tokenizer);
    if (!tokenizer->IsEnd()) {
        throw SyntaxError("");
    }
    return result;
}