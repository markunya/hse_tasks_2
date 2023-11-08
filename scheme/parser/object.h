#pragma once

#include <memory>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual const std::type_info& Type() const = 0;
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number() = delete;

    Number(int x) : value_(x) {
    }

    int GetValue() const {
        return value_;
    }

    virtual const std::type_info& Type() const {
        return typeid(Number);
    }

private:
    int value_;
};

class Boolean : public Object {
public:
    Boolean() = delete;

    Boolean(bool x) : value_(x) {
    }

    bool GetValue() const {
        return value_;
    }

    virtual const std::type_info& Type() const {
        return typeid(Boolean);
    }

private:
    bool value_;
};

class Symbol : public Object {
public:
    Symbol() = delete;

    Symbol(std::string x) : str_(std::move(x)) {
    }

    const std::string& GetName() const {
        return str_;
    }

    virtual const std::type_info& Type() const {
        return typeid(Symbol);
    }

private:
    std::string str_;
};

class Cell : public Object {
public:
    Cell() = delete;

    Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second = nullptr) : first_(first), second_(second) {
    }

    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }

    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }

    virtual const std::type_info& Type() const {
        return typeid(Cell);
    }

private:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return obj->Type() == typeid(T);
}
