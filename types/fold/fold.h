#pragma once


#include <vector>

struct Sum {
    Sum() = default;
    template <typename T, typename... Args>
    void operator()(T& res, const Args&... args) {
        ((res += args), ...);
    }
};

struct Prod {
    Prod() = default;
    template <typename T, typename... Args>
    void operator()(T& res, const Args&... args) {
        ((res *= args), ...);
    }
};

struct Concat {
    Concat() = default;
    template <typename T, typename... Args>
    void operator()(std::vector<T>& res, Args&... args) {
        (res.insert(res.end(), args.begin(), args.end()), ...);
    }
};

template <class Iterator, class T, class BinaryOp>
T Fold(Iterator first, Iterator last, T init, BinaryOp func) {
    T result = init;
    while (first != last) {
        func(result, *first);
        ++first;
    }
    return result;
}

class Length {
public:
    Length(int* count) : count_(count) {
    }

    template <typename... Args>
    void operator()(Args... args) {
        (++(*count_), (args, ...));
    }

private:
    int* count_;
};
