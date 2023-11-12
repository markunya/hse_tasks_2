#include "object.h"

#include "functions.h"
#include <unordered_map>
#include "scope.h"

struct ObjectImpl {
    ObjectImpl() = default;
    void Set(void* ptr) {
        if (!ptr) {
            scope_ = nullptr;
            return;
        }
        scope_ = reinterpret_cast<Scope*>(ptr)->GetPtr();
    }

    void* Get() {
        return reinterpret_cast<void*>(scope_.get());
    }

    std::shared_ptr<Scope> scope_ = nullptr;
};

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

void* Object::GetScope() {
    return impl_->Get();
}

void Object::SetScope(void* ptr) {
    impl_->Set(ptr);
    auto t = dynamic_cast<Cell*>(this);
    if (t) {
        if (t->GetFirst()) {
            t->GetFirst()->SetScope(ptr);
        }
        if (t->GetSecond()) {
            t->GetSecond()->SetScope(ptr);
        }
    }
}

Object::Object() : impl_(std::make_unique<ObjectImpl>()) {
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

Object::~Object() = default;

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
    auto t = dynamic_cast<LambdaFunction*>(function.get());
    if (t) {
        t->SetParentsScope(GetScope());
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

void Cell::SetFirst(std::shared_ptr<Object> obj) {
    first_ = obj;
}

void Cell::SetSecond(std::shared_ptr<Object> obj) {
    second_ = obj;
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
    auto res = impl_->scope_->Get(name_);
    return res;
}

std::string Symbol::Serialize() const {
    return name_;
}

std::shared_ptr<Object> Symbol::Clone() {
    return std::make_shared<Symbol>(name_);
}

void Symbol::DefineVariable(std::shared_ptr<Object> obj) {
    impl_->scope_->Define(name_, obj);
}

void Symbol::SetVariable(std::shared_ptr<Object> obj) {
    impl_->scope_->Set(name_, obj);
}

Symbol::~Symbol() = default;
