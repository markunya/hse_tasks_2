#pragma once

#include <memory>
#include "error.h"

struct ObjectImpl;

class Object : public std::enable_shared_from_this<Object> {
public:
    Object();

    virtual const std::type_info& Type() const;

    virtual std::shared_ptr<Object> Calculate();

    virtual std::string Serialize() const;

    virtual std::shared_ptr<Object> Clone();

    virtual std::shared_ptr<Object> Apply(std::shared_ptr<Object>);

    void SetScope(void* ptr);

    void* GetScope();

    virtual ~Object();

protected:
    std::unique_ptr<ObjectImpl> impl_;
};

class Boolean : public Object {
public:
    Boolean() = delete;

    Boolean(bool x);

    bool GetValue() const;

    const std::type_info& Type() const override;

    std::shared_ptr<Object> Calculate() override;

    std::shared_ptr<Object> Clone() override;

    std::string Serialize() const override;

private:
    bool value_;
};

class Number : public Object {
public:
    Number() = delete;

    Number(int x);

    int GetValue() const;

    const std::type_info& Type() const override;

    std::shared_ptr<Object> Calculate() override;

    std::shared_ptr<Object> Clone() override;

    std::string Serialize() const override;

private:
    int value_;
};

class Cell : public Object {
public:
    Cell() = delete;

    Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second = nullptr);

    std::shared_ptr<Object> GetFirst() const;

    void SetFirst(std::shared_ptr<Object> obj);

    std::shared_ptr<Object> GetSecond() const;

    void SetSecond(std::shared_ptr<Object> obj);

    const std::type_info& Type() const override;

    std::shared_ptr<Object> Calculate() override;

    std::shared_ptr<Object> Clone() override;

    std::string Serialize() const override;

private:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
};

class Symbol : public Object {
public:
    Symbol() = delete;

    Symbol(std::string x);

    const std::string& GetName() const;

    const std::type_info& Type() const override;

    std::shared_ptr<Object> Calculate() override;

    std::shared_ptr<Object> Clone() override;

    void DefineVariable(std::shared_ptr<Object> obj);

    void SetVariable(std::shared_ptr<Object> obj);

    std::string Serialize() const override;

    ~Symbol();

private:
    std::string name_;
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