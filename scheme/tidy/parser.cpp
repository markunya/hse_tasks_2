#include <parser.h>

std::shared_ptr<Object> LambdaSugarRebuilder(std::shared_ptr<Object> obj) {
    auto root = dynamic_cast<Cell*>(obj.get());
    auto right = dynamic_cast<Cell*>(root->GetFirst().get());
    if (!right) {
        throw SyntaxError("Invalid syntax for lambda");
    }
    std::shared_ptr<Object> name = right->GetFirst();
    std::shared_ptr<Object> args = right->GetSecond();
    std::shared_ptr<Object> body = root->GetSecond();
    return std::make_shared<Cell>(
        name, std::make_shared<Cell>(std::make_shared<Cell>(std::make_shared<Symbol>("lambda"),
                                                            std::make_shared<Cell>(args, body))));
}

std::shared_ptr<Object> ReadDefault(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("");
    }
    Token token = tokenizer->GetToken();
    if (std::holds_alternative<QuoteToken>(token)) {
        tokenizer->Next();
        if (tokenizer->IsEnd()) {
            throw SyntaxError("expected expression after quote");
        }
        return std::make_shared<Cell>(std::make_shared<Symbol>("quote"),
                                      std::make_shared<Cell>(ReadDefault(tokenizer)));
    }
    if (std::holds_alternative<ConstantToken>(token)) {
        tokenizer->Next();
        return std::make_shared<Number>(std::get<ConstantToken>(token).value);
    }
    if (std::holds_alternative<SymbolToken>(token)) {
        tokenizer->Next();
        return std::make_shared<Symbol>(std::get<SymbolToken>(token).name);
    }
    if (std::holds_alternative<BooleanToken>(token)) {
        tokenizer->Next();
        return std::make_shared<Boolean>(std::get<BooleanToken>(token).value);
    }
    if (std::holds_alternative<DotToken>(token)) {
        tokenizer->Next();
        throw SyntaxError("dot");
    }
    if (std::holds_alternative<BracketToken>(token)) {
        if (std::get<BracketToken>(token) == BracketToken::OPEN) {
            tokenizer->Next();
            if (std::holds_alternative<DotToken>(tokenizer->GetToken())) {
                throw SyntaxError("dot after appeared after open bracket");
            }
            return ReadList(tokenizer);
        }
        tokenizer->Next();
        throw SyntaxError("troubles with brackets");
    }
    throw SyntaxError("");
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    try {
        auto first = ReadDefault(tokenizer);
        auto second = ReadList(tokenizer);
        if (first && first->Type() == typeid(Symbol)) {
            if (dynamic_cast<Symbol*>(first.get())->GetName() == "define") {
                Cell* a = dynamic_cast<Cell*>(second.get());
                if (a && a->GetFirst()->Type() == typeid(Cell)) {
                    second = LambdaSugarRebuilder(second);
                }
            }
        }
        return std::make_shared<Cell>(first, second);
    } catch (const SyntaxError& e) {
        if (strcmp(e.what(), "troubles with brackets") == 0) {
            return nullptr;
        }
        if (strcmp(e.what(), "dot") == 0) {
            std::shared_ptr<Object> res = nullptr;
            try {
                res = ReadDefault(tokenizer);
            } catch (const SyntaxError& e) {
                if (strcmp(e.what(), "troubles with brackets") == 0 ||
                    strcmp(e.what(), "dot") == 0) {
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