#pragma once

#include <vector>
#include <functional>
#include "object.h"
#include "scope.h"

struct QuoteFunction : public Object {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct IfFunction : public Object {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct VarsFunction : public Object {
    std::pair<Symbol*, std::shared_ptr<Object>> Helper(std::shared_ptr<Object> obj);
};

struct DefineFunction : public VarsFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct SetFunction : public VarsFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct UnaryFunction : public Object {
    virtual std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override = 0;
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> obj);
    virtual ~UnaryFunction() = default;
};

struct ConvolutionalFunction : public Object {
    virtual std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override = 0;
    void Helper(std::shared_ptr<Object> obj, std::vector<std::shared_ptr<Object>>& vec);
    void TypesChecker(std::vector<std::shared_ptr<Object>>& vec,
                      const std::type_info& required_type);
    virtual ~ConvolutionalFunction() = default;
};

struct IsSimpleTypeFunction : public UnaryFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;

    virtual const std::type_info& GetType() = 0;
    virtual ~IsSimpleTypeFunction() = default;
};

struct IsBooleanFunction : public IsSimpleTypeFunction {
    const std::type_info& GetType() override;
};

struct IsNumberFunction : public IsSimpleTypeFunction {
    const std::type_info& GetType() override;
};

struct NotFunction : public UnaryFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct AndFunction : public ConvolutionalFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct OrFunction : public ConvolutionalFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct EqualFunction : public ConvolutionalFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct LessFunction : public ConvolutionalFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct BiggerFunction : public ConvolutionalFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct LessOrEqualFunction : public ConvolutionalFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct BiggerOrEqualFunction : public ConvolutionalFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct PlusFunction : public ConvolutionalFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct MultiplyFunction : public ConvolutionalFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct MinusFunction : public ConvolutionalFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct DivideFunction : public ConvolutionalFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct MaxFunction : public ConvolutionalFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct MinFunction : public ConvolutionalFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct AbsFunction : public UnaryFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct RequiresListArgumentFunction : public Object {
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> obj);
    virtual ~RequiresListArgumentFunction() = default;
};

struct IsPairFunction : public RequiresListArgumentFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct IsNullFunction : public RequiresListArgumentFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct IsListFunction : public RequiresListArgumentFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj) override;
};

struct CarFunction : public RequiresListArgumentFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj);
};

struct CdrFunction : public RequiresListArgumentFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj);
};

struct ConsFunction : public Object {
    void Helper(std::shared_ptr<Object> obj, std::vector<std::shared_ptr<Object>>& vec);
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj);
};

struct ListFunction : public Object {
    void Helper(std::shared_ptr<Object> obj);
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj);
};

struct ToWorkWithListFunction : public Object {
    std::shared_ptr<Object> Helper(std::shared_ptr<Object> obj, size_t& index);
    virtual ~ToWorkWithListFunction() = default;
};

struct ListRefFunction : public ToWorkWithListFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj);
};

struct ListTailFunction : public ToWorkWithListFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj);
};

struct LambdaFunction : public Object {
public:
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj);

    void SetParentsScope(void* ptr);

private:
    std::shared_ptr<Scope> scope_;
};

struct Lambda : public Object {
public:
    Lambda(std::shared_ptr<Object> args, std::shared_ptr<Object> body,
           std::shared_ptr<Scope> parents_scope);

    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj);

private:
    void FillScope(std::shared_ptr<Object> obj,
                   std::vector<std::pair<std::string, std::shared_ptr<Object>>>& remember);

    std::shared_ptr<Object> instructions_;
    std::vector<std::string> arguments_;
    std::shared_ptr<Scope> scope_;
};

struct ModifyConsFunction : public Object {
    void Helper(std::shared_ptr<Object> obj, std::vector<std::shared_ptr<Object>>& vec);
    virtual ~ModifyConsFunction() = default;
};

struct SetCarFunction : public ModifyConsFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj);
};

struct SetCdrFunction : public ModifyConsFunction {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj);
};

struct IsSymbolFunction : public Object {
    std::shared_ptr<Object> Apply(std::shared_ptr<Object> obj);
};