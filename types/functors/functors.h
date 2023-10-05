#pragma once

#include <vector>

template <class Functor>
class ReverseBinaryFunctor {
public:
    ReverseBinaryFunctor(Functor f) : f_(f) {
    }

    template <typename T, typename U>
    bool operator()(T&& x, U&& y) {
        return f_(y, x);
    }

private:
    Functor f_;
};

template <class Functor>
class ReverseUnaryFunctor {
public:
    ReverseUnaryFunctor(Functor f) : f_(f) {
    }
    template <typename T>
    bool operator()(T&& x) {
        return (!f_(x));
    }

private:
    Functor f_;
};

template <class Functor>
ReverseUnaryFunctor<Functor> MakeReverseUnaryFunctor(Functor functor) {
    return ReverseUnaryFunctor<Functor>(functor);
}

template <class Functor>
ReverseBinaryFunctor<Functor> MakeReverseBinaryFunctor(Functor functor) {
    return ReverseBinaryFunctor<Functor>(functor);
}
