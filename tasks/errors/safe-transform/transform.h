#pragma once
#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>

template <typename InputIterator, typename Predicate, typename Functor>
void TransformIf(InputIterator begin, InputIterator end, Predicate p, Functor f) {
    std::vector<typename std::iterator_traits<InputIterator>::value_type> temp;
    typename std::iterator_traits<InputIterator>::value_type copy;
    for (auto it = begin; it != end; ++it) {
        bool pit;
        try {
            pit = p(*it);
        } catch (...) {
            pit = p(*it);
        }
        if (pit) {
            try {
                copy = *it;
            } catch (...) {
                copy = *it;
            }
            f(copy);
            try {
                temp.push_back(copy);
            } catch (...) {
                temp.push_back(copy);
            }
        }
    }
    std::cout << "Loop1 ends\n";
    auto temp_it = temp.begin();
    for (auto it = begin; it != end && temp_it != temp.end(); ++it) {
        bool pit;
        try {
            pit = p(*it);
        } catch (...) {
            pit = p(*it);
        }
        if (pit) {
            try {
                *it = *temp_it;
            } catch (...) {
                *it = *temp_it;
            }
            ++temp_it;
        }
    }
    std::cout << "Transform ends\n";
}
