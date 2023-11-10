#include <sstream>
#include "scheme.h"
#include "tokenizer.h"
#include "parser.h"

std::string Interpreter::Run(const std::string& str) {
    auto in_ast = BuildInAst(str);
    if (!in_ast) {
        throw RuntimeError("");
    }
    auto out_ast = BuildOutAst(in_ast);
    if (!out_ast) {
        return "()";
    }
    std::string result = out_ast->Serialize();
    return result;
}
std::shared_ptr<Object> Interpreter::BuildOutAst(std::shared_ptr<Object> ast) {
    return ast->Calculate();
}

std::shared_ptr<Object> Interpreter::BuildInAst(const std::string& str) {
    std::stringstream ss{str};
    std::istream* iss = &ss;
    Tokenizer tokenizer(iss);
    return Read(&tokenizer);
}
