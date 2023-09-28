#pragma once

#include <string>

// Should not allow reuse and yell under sanitizers.
// Fix the interface and implementation.
// AwesomeCallback should add "awesomeness".

class OneTimeCallback {
public:
    virtual ~OneTimeCallback() = default;
    virtual std::string operator()() const&& = 0;
};

// Implement ctor, operator(), maybe something else...
class AwesomeCallback : public OneTimeCallback {
public:
    AwesomeCallback() = default;
    AwesomeCallback(const std::string& str) : str_(str + "awesomeness") {
    }
    std::string operator()() const&&;

private:
    std::string str_ = "awesomeness";
};
