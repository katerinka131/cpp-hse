#pragma once

#include <vector>
#include <cstdlib>
struct Sum {
    int operator()(auto a, auto b) {
        return a + b;
    }
};

struct Prod {
    int operator()(auto a, auto b) {
        return a * b;
    }
};

struct Concat {
    auto operator()(auto a, auto b) {
        auto res(a);
        for (size_t i = 0; i < b.size(); i++) {
            res.push_back(b[i]);
        }
        return res;
    }
};

template <class Iterator, class T, class BinaryOp>
T Fold(Iterator first, Iterator last, T init, BinaryOp func) {
    while (first != last) {
        init = func(init, *first);
        first++;
        //        last--;
    }
    return init;
}

class Length {
    int *len_;

public:
    Length(int *len) : len_(len) {
    }
    auto operator()(auto &a, auto &b) {
        ++(*len_);
        return a;
    }
};