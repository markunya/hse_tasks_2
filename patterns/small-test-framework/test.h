#pragma once

#include <map>
#include <exception>

class AbstractTest {
public:
    virtual void SetUp() = 0;
    virtual void TearDown() = 0;
    virtual void Run() = 0;
    virtual ~AbstractTest() {
    }
};

struct FullMatch {
    FullMatch(const std::string& str) : str(str) {
    }
    bool operator()(const std::string& other) {
        return (other == str);
    }
    std::string str;
};

struct Substr {
    Substr(const std::string& str) : str(str) {
    }
    bool operator()(const std::string& other) {
        return (other.find(str) != std::string::npos);
    }
    std::string str;
};

class TestRegistry {
public:
    template <class TestClass>
    void RegisterClass(const std::string& class_name) {
        if (!registry_.count(class_name)) {
            registry_[class_name].reset(new TestBuilder<TestClass>());
        }
    }

    std::unique_ptr<AbstractTest> CreateTest(const std::string& class_name) {
        if (!registry_.count(class_name)) {
            throw std::out_of_range("Test is not registered");
        }
        return registry_[class_name]->Create();
    }

    void RunTest(const std::string& test_name) {
        std::unique_ptr<AbstractTest> ptr = CreateTest(test_name);
        ptr->SetUp();
        try {
            ptr->Run();
        } catch (...) {
            ptr->TearDown();
            throw;
        }
        ptr->TearDown();
    }

    template <class Predicate>
    std::vector<std::string> ShowTests(Predicate callback) const {
        std::vector<std::string> result;
        for (auto& [str, _] : registry_) {
            if (callback(str)) {
                result.emplace_back(str);
            }
        }
        return result;
    }

    std::vector<std::string> ShowAllTests() const {
        std::vector<std::string> result;
        result.reserve(registry_.size());
        for (auto& [str, _] : registry_) {
            result.emplace_back(str);
        }
        return result;
    }

    template <class Predicate>
    void RunTests(Predicate callback) {
        for (auto& [str, _] : registry_) {
            if (callback(str)) {
                RunTest(str);
            }
        }
    }

    void Clear() {
        registry_.clear();
    }

    static TestRegistry& Instance() {
        if (!instance) {
            instance = std::unique_ptr<TestRegistry>(new TestRegistry());
        }
        return *instance;
    }

protected:
    TestRegistry() = default;

private:
    struct TestBuilderBase {
        virtual std::unique_ptr<AbstractTest> Create() = 0;
        virtual ~TestBuilderBase() = default;
    };

    template <typename TestClass>
    struct TestBuilder : TestBuilderBase {
        std::unique_ptr<AbstractTest> Create() override {
            return std::unique_ptr<AbstractTest>(new TestClass());
        }
    };

    std::map<std::string, std::unique_ptr<TestBuilderBase>> registry_;
    inline static std::unique_ptr<TestRegistry> instance = nullptr;
};
