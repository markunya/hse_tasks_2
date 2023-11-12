#include "functions.h"

std::shared_ptr<Object> QuoteFunction::Apply(std::shared_ptr<Object> obj) {
    if (!obj) {
        return nullptr;
    }
    auto t = dynamic_cast<Cell*>(obj.get());
    if (!t) {
        throw RuntimeError("");
    }
    if (!t->GetFirst()) {
        return nullptr;
    }
    return t->GetFirst()->Clone();
}

std::shared_ptr<Object> UnaryFunction::Helper(std::shared_ptr<Object> obj) {
    if (!obj || obj->Type() != typeid(Cell)) {
        throw RuntimeError("");
    }
    Cell* cell = dynamic_cast<Cell*>(obj.get());
    if (cell->GetSecond()) {
        throw RuntimeError("");
    }
    return cell->GetFirst();
}

std::shared_ptr<Object> IsSimpleTypeFunction::Apply(std::shared_ptr<Object> obj) {
    if (Helper(obj)->Type() == GetType()) {
        return std::make_shared<Boolean>(true);
    }
    return std::make_shared<Boolean>(false);
}

const std::type_info& IsBooleanFunction::GetType() {
    return typeid(Boolean);
}

const std::type_info& IsNumberFunction::GetType() {
    return typeid(Number);
}

std::shared_ptr<Object> NotFunction::Apply(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> t = Helper(obj);
    if (t->Type() != typeid(Boolean)) {
        return std::make_shared<Boolean>(false);
    }
    Boolean* x = dynamic_cast<Boolean*>(t.get());
    if (x->GetValue()) {
        return std::make_shared<Boolean>(false);
    }
    return std::make_shared<Boolean>(true);
}

void ConvolutionalFunction::Helper(std::shared_ptr<Object> obj,
                                   std::vector<std::shared_ptr<Object>>& vec) {
    Object* current = obj.get();
    while (current) {
        if (current->Type() != typeid(Cell)) {
            throw RuntimeError("");
        }
        Cell* current_cell = dynamic_cast<Cell*>(current);
        if (!current_cell->GetFirst()) {
            throw RuntimeError("");
        }
        auto el = current_cell->GetFirst()->Calculate();
        vec.emplace_back(el);
        current = current_cell->GetSecond().get();
    }
}

void ConvolutionalFunction::TypesChecker(std::vector<std::shared_ptr<Object>>& vec,
                                         const std::type_info& required_type) {
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i]->Type() != required_type) {
            throw RuntimeError("invalid types");
        }
    }
}

std::shared_ptr<Object> AndFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    try {
        Helper(obj, vec);
    } catch (SyntaxError& e) {
        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i]->Type() == typeid(Boolean)) {
                auto b = dynamic_cast<Boolean*>(vec[i].get());
                if (!b->GetValue()) {
                    return vec[i]->Clone();
                }
            }
        }
        throw;
    } catch (NameError& e) {
        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i]->Type() == typeid(Boolean)) {
                auto b = dynamic_cast<Boolean*>(vec[i].get());
                if (!b->GetValue()) {
                    return vec[i]->Clone();
                }
            }
        }
        throw;
    }
    if (vec.empty()) {
        return std::make_shared<Boolean>(true);
    }
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i]->Type() == typeid(Boolean)) {
            auto b = dynamic_cast<Boolean*>(vec[i].get());
            if (!b->GetValue()) {
                return vec[i]->Clone();
            }
        }
    }
    return vec.back()->Clone();
}

std::shared_ptr<Object> OrFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    try {
        Helper(obj, vec);
    } catch (SyntaxError& e) {
        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i]->Type() != typeid(Boolean)) {
                return vec[i]->Clone();
            }
            auto b = dynamic_cast<Boolean*>(vec[i].get());
            if (b->GetValue()) {
                return vec[i]->Clone();
            }
        }
        throw;
    } catch (NameError& e) {
        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i]->Type() != typeid(Boolean)) {
                return vec[i]->Clone();
            }
            auto b = dynamic_cast<Boolean*>(vec[i].get());
            if (b->GetValue()) {
                return vec[i]->Clone();
            }
        }
        throw;
    }
    if (vec.empty()) {
        return std::make_shared<Boolean>(false);
    }
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i]->Type() != typeid(Boolean)) {
            return vec[i]->Clone();
        }
        auto b = dynamic_cast<Boolean*>(vec[i].get());
        if (b->GetValue()) {
            return vec[i]->Clone();
        }
    }
    return std::make_shared<Boolean>(false);
}

std::shared_ptr<Object> EqualFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    Helper(obj, vec);
    if (vec.size() <= 1) {
        return std::make_shared<Boolean>(true);
    }
    TypesChecker(vec, typeid(Number));
    auto base = dynamic_cast<Number*>(vec[0].get());
    for (size_t i = 1; i < vec.size(); ++i) {
        auto t = dynamic_cast<Number*>(vec[i].get());
        if (base->GetValue() != t->GetValue()) {
            return std::make_shared<Boolean>(false);
        }
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> LessFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    Helper(obj, vec);
    if (vec.size() <= 1) {
        return std::make_shared<Boolean>(true);
    }
    TypesChecker(vec, typeid(Number));
    auto base = dynamic_cast<Number*>(vec[0].get());
    for (size_t i = 1; i < vec.size(); ++i) {
        auto t = dynamic_cast<Number*>(vec[i].get());
        if (base->GetValue() >= t->GetValue()) {
            return std::make_shared<Boolean>(false);
        }
        base = t;
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> BiggerFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    Helper(obj, vec);
    if (vec.size() <= 1) {
        return std::make_shared<Boolean>(true);
    }
    TypesChecker(vec, typeid(Number));
    auto base = dynamic_cast<Number*>(vec[0].get());
    for (size_t i = 1; i < vec.size(); ++i) {
        auto t = dynamic_cast<Number*>(vec[i].get());
        if (base->GetValue() <= t->GetValue()) {
            return std::make_shared<Boolean>(false);
        }
        base = t;
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> LessOrEqualFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    Helper(obj, vec);
    if (vec.size() <= 1) {
        return std::make_shared<Boolean>(true);
    }
    TypesChecker(vec, typeid(Number));
    auto base = dynamic_cast<Number*>(vec[0].get());
    for (size_t i = 1; i < vec.size(); ++i) {
        auto t = dynamic_cast<Number*>(vec[i].get());
        if (base->GetValue() > t->GetValue()) {
            return std::make_shared<Boolean>(false);
        }
        base = t;
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> BiggerOrEqualFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    Helper(obj, vec);
    if (vec.size() <= 1) {
        return std::make_shared<Boolean>(true);
    }
    TypesChecker(vec, typeid(Number));
    auto base = dynamic_cast<Number*>(vec[0].get());
    for (size_t i = 1; i < vec.size(); ++i) {
        auto t = dynamic_cast<Number*>(vec[i].get());
        if (base->GetValue() < t->GetValue()) {
            return std::make_shared<Boolean>(false);
        }
        base = t;
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> PlusFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    Helper(obj, vec);
    TypesChecker(vec, typeid(Number));
    int result = 0;
    for (size_t i = 0; i < vec.size(); ++i) {
        auto num = dynamic_cast<Number*>(vec[i].get());
        result += num->GetValue();
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> MultiplyFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    Helper(obj, vec);
    TypesChecker(vec, typeid(Number));
    int result = 1;
    for (size_t i = 0; i < vec.size(); ++i) {
        auto num = dynamic_cast<Number*>(vec[i].get());
        result *= num->GetValue();
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> MinusFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    Helper(obj, vec);
    TypesChecker(vec, typeid(Number));
    if (vec.size() <= 1) {
        throw RuntimeError("too few arguments for minus");
    }
    int result = dynamic_cast<Number*>(vec[0].get())->GetValue();
    for (size_t i = 1; i < vec.size(); ++i) {
        auto num = dynamic_cast<Number*>(vec[i].get());
        result -= num->GetValue();
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> DivideFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    Helper(obj, vec);
    TypesChecker(vec, typeid(Number));
    if (vec.size() <= 1) {
        throw RuntimeError("too few arguments for minus");
    }
    int result = dynamic_cast<Number*>(vec[0].get())->GetValue();
    for (size_t i = 1; i < vec.size(); ++i) {
        auto num = dynamic_cast<Number*>(vec[i].get());
        result /= num->GetValue();
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> MaxFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    Helper(obj, vec);
    TypesChecker(vec, typeid(Number));
    if (vec.empty()) {
        throw RuntimeError("to few arguments for max");
    }
    int result = dynamic_cast<Number*>(vec[0].get())->GetValue();
    for (size_t i = 0; i < vec.size(); ++i) {
        auto num = dynamic_cast<Number*>(vec[i].get());
        result = std::max(num->GetValue(), result);
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> MinFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    Helper(obj, vec);
    TypesChecker(vec, typeid(Number));
    if (vec.empty()) {
        throw RuntimeError("to few arguments for min");
    }
    int result = dynamic_cast<Number*>(vec[0].get())->GetValue();
    for (size_t i = 0; i < vec.size(); ++i) {
        auto num = dynamic_cast<Number*>(vec[i].get());
        result = std::min(num->GetValue(), result);
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> AbsFunction::Apply(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> num = Helper(obj);
    if (num->Type() != typeid(Number)) {
        throw RuntimeError("abs argument must be a number");
    }
    int val = dynamic_cast<Number*>(num.get())->GetValue();
    return std::make_shared<Number>(abs(val));
}

std::shared_ptr<Object> RequiresListArgumentFunction::Helper(std::shared_ptr<Object> obj) {
    if (!obj) {
        throw RuntimeError("");
    }
    if (obj->Type() != typeid(Cell)) {
        return obj;
    }
    Cell* cell = dynamic_cast<Cell*>(obj.get());
    if (cell->GetSecond()) {
        throw RuntimeError("to many arguments");
    }
    return cell->GetFirst()->Calculate();
}

std::shared_ptr<Object> IsPairFunction::Apply(std::shared_ptr<Object> obj) {
    auto t = Helper(obj);
    if (!t || t->Type() != typeid(Cell)) {
        return std::make_shared<Boolean>(false);
    }
    auto first_level = dynamic_cast<Cell*>(t.get());
    if (!first_level->GetSecond()) {
        return std::make_shared<Boolean>(false);
    }
    if (first_level->GetSecond()->Type() != typeid(Cell)) {
        return std::make_shared<Boolean>(true);
    }
    auto second_level = dynamic_cast<Cell*>(first_level->GetSecond().get());
    if (second_level->GetSecond()) {
        return std::make_shared<Boolean>(false);
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> IsNullFunction::Apply(std::shared_ptr<Object> obj) {
    auto t = Helper(obj);
    if (!t) {
        return std::make_shared<Boolean>(true);
    }
    return std::make_shared<Boolean>(false);
}

std::shared_ptr<Object> IsListFunction::Apply(std::shared_ptr<Object> obj) {
    auto t = Helper(obj);
    if (!t) {
        return std::make_shared<Boolean>(true);
    }
    if (t->Type() != typeid(Cell)) {
        return std::make_shared<Boolean>(false);
    }
    auto current = dynamic_cast<Cell*>(t.get());
    while (current) {
        if (!current->GetSecond()) {
            return std::make_shared<Boolean>(true);
        }
        if (current->GetSecond()->Type() != typeid(Cell)) {
            return std::make_shared<Boolean>(false);
        }
        current = dynamic_cast<Cell*>(current->GetSecond().get());
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> CarFunction::Apply(std::shared_ptr<Object> obj) {
    auto t = Helper(obj);
    if (!t || t->Type() != typeid(Cell)) {
        throw RuntimeError("Invalid argument for car");
    }
    auto cell = dynamic_cast<Cell*>(t.get());
    if (!cell->GetFirst()) {
        throw RuntimeError("");
    }
    return cell->GetFirst();
}

std::shared_ptr<Object> CdrFunction::Apply(std::shared_ptr<Object> obj) {
    auto t = Helper(obj);
    if (!t || t->Type() != typeid(Cell)) {
        throw RuntimeError("Invalid argument for car");
    }
    auto cell = dynamic_cast<Cell*>(t.get());
    if (!cell->GetSecond()) {
        return nullptr;
    }
    return cell->GetSecond();
}

void ConsFunction::Helper(std::shared_ptr<Object> obj, std::vector<std::shared_ptr<Object>>& vec) {
    Object* current = obj.get();
    while (current) {
        if (current->Type() != typeid(Cell)) {
            throw RuntimeError("");
        }
        Cell* current_cell = dynamic_cast<Cell*>(current);
        auto el = current_cell->GetFirst();
        vec.emplace_back(el);
        current = current_cell->GetSecond().get();
    }
}

std::shared_ptr<Object> ConsFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    Helper(obj, vec);
    if (vec.size() != 2) {
        throw RuntimeError("expected two arguments for cons");
    }
    auto first = vec[0]->Calculate();
    auto second = vec[1]->Calculate();
    return std::make_shared<Cell>(first, second);
}

void ListFunction::Helper(std::shared_ptr<Object> obj) {
    Object* current = obj.get();
    while (current) {
        if (current->Type() != typeid(Cell)) {
            throw RuntimeError("");
        }
        Cell* current_cell = dynamic_cast<Cell*>(current);
        auto el = current_cell->GetFirst();
        current = current_cell->GetSecond().get();
    }
}

std::shared_ptr<Object> ListFunction::Apply(std::shared_ptr<Object> obj) {
    Helper(obj);
    if (!obj) {
        return nullptr;
    }
    return obj->Clone();
}

std::shared_ptr<Object> ToWorkWithListFunction::Helper(std::shared_ptr<Object> obj, size_t& index) {
    if (!obj) {
        throw RuntimeError("");
    }
    if (obj->Type() != typeid(Cell)) {
        throw RuntimeError("Invalid arguments");
    }
    Cell* cell = dynamic_cast<Cell*>(obj.get());
    if (!cell->GetSecond() || cell->GetSecond()->Type() != typeid(Cell)) {
        throw RuntimeError("Invalid arguments");
    }
    auto cell_with_index = dynamic_cast<Cell*>(cell->GetSecond().get());
    if (!cell_with_index->GetFirst() || cell_with_index->GetFirst()->Type() != typeid(Number) ||
        cell_with_index->GetSecond()) {
        throw RuntimeError("Invalid arguments");
    }
    auto index_obj = dynamic_cast<Number*>(cell_with_index->GetFirst().get());
    index = static_cast<size_t>(index_obj->GetValue());
    return cell->GetFirst()->Calculate();
}

std::shared_ptr<Object> ListRefFunction::Apply(std::shared_ptr<Object> obj) {
    size_t index = 0;
    auto t = Helper(obj, index);
    auto current = t.get();
    while (current && index > 0) {
        if (current->Type() != typeid(Cell)) {
            throw RuntimeError("");
        }
        Cell* current_cell = dynamic_cast<Cell*>(current);
        current = current_cell->GetSecond().get();
        --index;
    }
    if (!current || current->Type() != typeid(Cell)) {
        throw RuntimeError("");
    }
    Cell* target_cell = dynamic_cast<Cell*>(current);
    return target_cell->GetFirst();
}

std::shared_ptr<Object> ListTailFunction::Apply(std::shared_ptr<Object> obj) {
    size_t index = 0;
    auto t = Helper(obj, index);
    auto current = t.get();
    while (current && index > 0) {
        if (current->Type() != typeid(Cell)) {
            throw RuntimeError("");
        }
        Cell* current_cell = dynamic_cast<Cell*>(current);
        current = current_cell->GetSecond().get();
        --index;
    }
    if (index > 0) {
        throw RuntimeError("");
    }
    if (!current) {
        return nullptr;
    }
    return current->Clone();
}

std::shared_ptr<Object> IfFunction::Apply(std::shared_ptr<Object> obj) {
    if (!obj || obj->Type() != typeid(Cell)) {
        throw SyntaxError("Invalid syntax for if");
    }
    Cell* current_cell = dynamic_cast<Cell*>(obj.get());
    Cell* checker = current_cell;
    size_t depth = 0;
    while (checker) {
        checker = dynamic_cast<Cell*>(checker->GetSecond().get());
        ++depth;
        if (depth == 4) {
            throw SyntaxError("Invalid syntax for if");
        }
    }
    if (depth == 1) {
        throw SyntaxError("Invalid syntax for if");
        ;
    }
    if (!current_cell->GetFirst()) {
        throw RuntimeError("If statement used not correct");
    }
    auto b = current_cell->GetFirst()->Calculate();
    bool condition = false;
    if (b->Type() != typeid(Boolean)) {
        condition = true;
    } else {
        if (dynamic_cast<Boolean*>(b.get())->GetValue()) {
            condition = true;
        }
    }
    for (size_t i = 0; i < 2 - static_cast<size_t>(condition); ++i) {
        current_cell = dynamic_cast<Cell*>(current_cell->GetSecond().get());
    }
    if (!current_cell) {
        return nullptr;
    }
    if (!current_cell->GetFirst()) {
        throw RuntimeError("");
    }
    auto result = current_cell->GetFirst()->Calculate();
    return result;
}

std::shared_ptr<Object> DefineFunction::Apply(std::shared_ptr<Object> obj) {
    auto [name, value] = Helper(obj);
    name->DefineVariable(value);
    return nullptr;
}

std::shared_ptr<Object> SetFunction::Apply(std::shared_ptr<Object> obj) {
    auto [name, value] = Helper(obj);
    name->SetVariable(value);
    return nullptr;
}

std::pair<Symbol*, std::shared_ptr<Object>> VarsFunction::Helper(std::shared_ptr<Object> obj) {
    if (!obj || obj->Type() != typeid(Cell)) {
        throw SyntaxError("Invalid syntax for define");
    }
    auto current_cell = dynamic_cast<Cell*>(obj.get());
    if (!current_cell->GetFirst() || current_cell->GetFirst()->Type() != typeid(Symbol)) {
        throw SyntaxError("Variable must be a symbol");
    }
    if (!current_cell->GetSecond() || current_cell->GetSecond()->Type() != typeid(Cell)) {
        throw SyntaxError("Invalid syntax for define");
    }
    auto next_cell = dynamic_cast<Cell*>(current_cell->GetSecond().get());
    if (!next_cell->GetFirst() || next_cell->GetSecond()) {
        throw SyntaxError("Invalid syntax for define");
    }
    auto name = dynamic_cast<Symbol*>(current_cell->GetFirst().get());
    return std::make_pair(name, next_cell->GetFirst()->Calculate());
}

std::shared_ptr<Object> LambdaFunction::Apply(std::shared_ptr<Object> obj) {
    Cell* root = dynamic_cast<Cell*>(obj.get());
    if (!root) {
        throw SyntaxError("Invalid syntax for lambda expression");
    }
    if (!root->GetSecond()) {
        throw SyntaxError("Invalid syntax for lambda expression");
    }
    auto result = std::make_shared<Lambda>(root->GetFirst(), root->GetSecond(), scope_);
    scope_ = nullptr;
    return result;
}

void LambdaFunction::SetParentsScope(void* ptr) {
    scope_ = reinterpret_cast<Scope*>(ptr)->shared_from_this();
}

Lambda::Lambda(std::shared_ptr<Object> args, std::shared_ptr<Object> body,
               std::shared_ptr<Scope> parents_scope)
    : instructions_(body), scope_(std::make_shared<Scope>(parents_scope)) {
    auto current = dynamic_cast<Cell*>(args.get());
    while (current) {
        if (current->GetFirst()->Type() != typeid(Symbol)) {
            throw SyntaxError("Arguments of lambda must be symbols");
        }
        auto var = dynamic_cast<Symbol*>(current->GetFirst().get());
        arguments_.emplace_back(var->GetName());
        current = dynamic_cast<Cell*>(current->GetSecond().get());
    }
    instructions_->SetScope(scope_.get());
}

std::shared_ptr<Object> Lambda::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::pair<std::string, std::shared_ptr<Object>>> remember;
    FillScope(obj, remember);
    auto current = dynamic_cast<Cell*>(instructions_.get());
    if (!current) {
        throw SyntaxError("Body of lambda is emtpy");
    }
    while (current->GetSecond()) {
        current->GetFirst()->Calculate();
        current = dynamic_cast<Cell*>(current->GetSecond().get());
    }
    if (!current->GetFirst()) {
        throw SyntaxError("");
    }
    auto result = current->GetFirst()->Calculate();
    for (size_t i = 0; i < remember.size(); ++i) {
        scope_->Define(remember[i].first, remember[i].second);
    }
    return result;
}

void Lambda::FillScope(std::shared_ptr<Object> obj,
                       std::vector<std::pair<std::string, std::shared_ptr<Object>>>& remember) {
    auto current = dynamic_cast<Cell*>(obj.get());
    size_t index = 0;
    while (current) {
        if (index == arguments_.size()) {
            throw RuntimeError("Invalid number of arguments in lambda");
        }
        if (scope_->Contains(arguments_[index])) {
            auto value = scope_->Get(arguments_[index]);
            remember.emplace_back(arguments_[index], value);
        }
        scope_->Define(arguments_[index], current->GetFirst()->Calculate());
        current = dynamic_cast<Cell*>(current->GetSecond().get());
        ++index;
    }
    if (index != arguments_.size()) {
        throw RuntimeError("Invalid number of arguments in lambda");
    }
}

void ModifyConsFunction::Helper(std::shared_ptr<Object> obj,
                                std::vector<std::shared_ptr<Object>>& vec) {
    Object* current = obj.get();
    while (current) {
        if (current->Type() != typeid(Cell)) {
            throw RuntimeError("");
        }
        Cell* current_cell = dynamic_cast<Cell*>(current);
        auto el = current_cell->GetFirst();
        vec.emplace_back(el);
        current = current_cell->GetSecond().get();
    }
}

std::shared_ptr<Object> SetCarFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    Helper(obj, vec);
    if (vec.size() != 2) {
        throw RuntimeError("expected two arguments for set-car! function");
    }
    auto first = vec[0]->Calculate();
    auto second = vec[1]->Calculate();
    auto cell = dynamic_cast<Cell*>(first.get());
    if (!cell) {
        throw RuntimeError("expected pair for first argument");
    }
    cell->SetFirst(second);
    return nullptr;
}

std::shared_ptr<Object> SetCdrFunction::Apply(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> vec;
    Helper(obj, vec);
    if (vec.size() != 2) {
        throw RuntimeError("expected two arguments for set-cdr! function");
    }
    auto first = vec[0]->Calculate();
    auto second = vec[1]->Calculate();
    auto cell = dynamic_cast<Cell*>(first.get());
    if (!cell) {
        throw RuntimeError("expected pair for first argument");
    }
    cell->SetSecond(second);
    return nullptr;
}

std::shared_ptr<Object> IsSymbolFunction::Apply(std::shared_ptr<Object> obj) {
    auto cell = dynamic_cast<Cell*>(obj.get());
    if (!cell || cell->GetSecond()) {
        throw SyntaxError("");
    }
    auto f = cell->GetFirst();
    auto t = f->Calculate();
    return std::make_shared<Boolean>(t->Type() == typeid(Symbol));
}
