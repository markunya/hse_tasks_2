#pragma once

#include <string>
#include "object.h"
#include "scope.h"
#include "functions.h"

class Interpreter {
public:
    std::string Run(const std::string&);

    std::shared_ptr<Object> BuildInAst(const std::string&);
    std::shared_ptr<Object> BuildOutAst(std::shared_ptr<Object>);

private:
    std::shared_ptr<Scope> global_scope_ = std::shared_ptr<Scope>(new Scope{
        {"quote", std::make_shared<QuoteFunction>()},
        {"boolean?", std::make_shared<IsBooleanFunction>()},
        {"and", std::make_shared<AndFunction>()},
        {"or", std::make_shared<OrFunction>()},
        {"not", std::make_shared<NotFunction>()},
        {"number?", std::make_shared<IsNumberFunction>()},
        {"+", std::make_shared<PlusFunction>()},
        {"-", std::make_shared<MinusFunction>()},
        {"/", std::make_shared<DivideFunction>()},
        {"*", std::make_shared<MultiplyFunction>()},
        {"=", std::make_shared<EqualFunction>()},
        {"<", std::make_shared<LessFunction>()},
        {">", std::make_shared<BiggerFunction>()},
        {"<=", std::make_shared<LessOrEqualFunction>()},
        {">=", std::make_shared<BiggerOrEqualFunction>()},
        {"min", std::make_shared<MinFunction>()},
        {"max", std::make_shared<MaxFunction>()},
        {"abs", std::make_shared<AbsFunction>()},
        {"list?", std::make_shared<IsListFunction>()},
        {"pair?", std::make_shared<IsPairFunction>()},
        {"null?", std::make_shared<IsNullFunction>()},
        {"cons", std::make_shared<ConsFunction>()},
        {"list", std::make_shared<ListFunction>()},
        {"car", std::make_shared<CarFunction>()},
        {"cdr", std::make_shared<CdrFunction>()},
        {"list-ref", std::make_shared<ListRefFunction>()},
        {"list-tail", std::make_shared<ListTailFunction>()},
        {"if", std::make_shared<IfFunction>()},
        {"define", std::make_shared<DefineFunction>()},
        {"set!", std::make_shared<SetFunction>()},
        {"lambda", std::make_shared<LambdaFunction>()},
        {"set-car!", std::make_shared<SetCarFunction>()},
        {"set-cdr!", std::make_shared<SetCdrFunction>()},
        {"symbol?", std::make_shared<IsSymbolFunction>()}
    });

    void SetGlobalScope(std::shared_ptr<Object> root);
};
