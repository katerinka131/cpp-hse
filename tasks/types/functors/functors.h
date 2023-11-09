#pragma once

#include <vector>

template <class Functor>
class ReverseBinaryFunctor {

public:
    ReverseBinaryFunctor(Functor func) {
        this->func_ = func;
    };
    auto operator()(auto a, auto b) {
        return func_(b, a);
    }

private:
    Functor func_;
};

template <class Functor>
class ReverseUnaryFunctor {

public:
    ReverseUnaryFunctor(Functor func) {
        this->func_ = func;
    };
    auto operator()(auto a) {
        return !(func_(a));
    }

private:
    Functor func_;
};

template <class Functor>
ReverseUnaryFunctor<Functor> MakeReverseUnaryFunctor(Functor functor) {
    return ReverseUnaryFunctor<Functor>(functor);
}

template <class Functor>
ReverseBinaryFunctor<Functor> MakeReverseBinaryFunctor(Functor functor) {
    return ReverseBinaryFunctor<Functor>(functor);
}