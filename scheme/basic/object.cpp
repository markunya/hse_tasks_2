#include "object.h"

#include "functions.h"
#include <unordered_map>

static std::unordered_map<std::string, std::shared_ptr<Object>> k_function_symbols = {
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
    {"list-tail", std::make_shared<ListTailFunction>()}};

const std::type_info& Object::Type() const {
    throw RuntimeError("Type is not implemented");
}

std::shared_ptr<Object> Object::Calculate() {
    throw RuntimeError("Calculate is not implemented");
}
std::string Object::Serialize() const {
    throw RuntimeError("Serialize is not implemented");
}
std::shared_ptr<Object> Object::Apply(std::shared_ptr<Object>) {
    throw RuntimeError("Apply is not implemented");
}
std::shared_ptr<Object> Object::Clone() {
    throw RuntimeError("Clone is not implemented");
}

Boolean::Boolean(bool x) : value_(x) {
}

bool Boolean::GetValue() const {
    return value_;
}

const std::type_info& Boolean::Type() const {
    return typeid(Boolean);
}
std::shared_ptr<Object> Boolean::Calculate() {
    return shared_from_this();
}
std::string Boolean::Serialize() const {
    if (value_) {
        return "#t";
    }
    return "#f";
}
std::shared_ptr<Object> Boolean::Clone() {
    return std::make_shared<Boolean>(value_);
}

Number::Number(int x) : value_(x) {
}

int Number::GetValue() const {
    return value_;
}

const std::type_info& Number::Type() const {
    return typeid(Number);
}

std::shared_ptr<Object> Number::Calculate() {
    return shared_from_this();
}

std::string Number::Serialize() const {
    return std::to_string(value_);
}

std::shared_ptr<Object> Number::Clone() {
    return std::make_shared<Number>(value_);
}

Cell::Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second)
    : first_(first), second_(second) {
}

std::shared_ptr<Object> Cell::GetFirst() const {
    return first_;
}

std::shared_ptr<Object> Cell::GetSecond() const {
    return second_;
}

const std::type_info& Cell::Type() const {
    return typeid(Cell);
}

std::shared_ptr<Object> Cell::Calculate() {
    if (!first_) {
        throw RuntimeError("");
    }
    std::shared_ptr<Object> function = first_->Calculate();
    if (!function) {
        throw RuntimeError("");
    }
    std::shared_ptr<Object> result = function->Apply(second_);
    return result;
}

std::string Cell::Serialize() const {
    std::string result = "(";
    if (first_) {
        result += first_->Serialize();
    } else {
        result += "()";
    }
    Object* current = second_.get();
    while (current) {
        if (current->Type() != Type()) {
            result += (" . " + current->Serialize());
            break;
        }
        Cell* current_cell = dynamic_cast<Cell*>(current);
        result += (' ' + current_cell->GetFirst()->Serialize());
        current = current_cell->second_.get();
    }
    result += ")";
    return result;
}

std::shared_ptr<Object> Cell::Clone() {
    if (!first_ && !second_) {
        return std::make_shared<Cell>(nullptr);
    }
    if (!first_) {
        return std::make_shared<Cell>(second_->Clone());
    }
    if (!second_) {
        return std::make_shared<Cell>(first_->Clone());
    }
    return std::make_shared<Cell>(first_->Clone(), second_->Clone());
}

Symbol::Symbol(std::string x) : name_(std::move(x)) {
}

const std::string& Symbol::GetName() const {
    return name_;
}

const std::type_info& Symbol::Type() const {
    return typeid(Symbol);
}

std::shared_ptr<Object> Symbol::Calculate() {
    if (!k_function_symbols.count(name_)) {
        throw SyntaxError("");
    }
    return k_function_symbols[name_];
}

std::string Symbol::Serialize() const {
    return name_;
}
std::shared_ptr<Object> Symbol::Clone() {
    return std::make_shared<Symbol>(name_);
}
