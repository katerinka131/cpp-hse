#pragma once
#include <iostream>
#include <type_traits>

template <typename T, size_t I, bool = std::is_empty_v<T> && !std::is_final_v<T>>
struct CompressedPairElement {
public:
    CompressedPairElement() noexcept(std::is_nothrow_default_constructible_v<T>)
        : dat_(T()) {}

    template <typename U>
    CompressedPairElement(U&& v)
        : dat_(std::forward<U>(v)) {}

    const T& Getts() const noexcept {
        return dat_;
    }
    T& Getts() noexcept {
        return dat_;
    }

private:
    T dat_;
};

template <typename T, size_t I>
struct CompressedPairElement<T, I, true> : public T {
public:
    CompressedPairElement() noexcept(std::is_nothrow_default_constructible_v<T>) {}

    template <typename U>
    CompressedPairElement(U&& v)
        : T(std::forward<U>(v)) {}

    const T& Getts() const noexcept {
        return *this;
    }
    T& Getts() noexcept {
        return *this;
    }

};

template <typename F, typename S>
class CompressedPair : private CompressedPairElement<F, 0>, private CompressedPairElement<S, 1> {

public:
    CompressedPair() {}

    template <typename R, typename U>
    CompressedPair(R&& first, U&& second)
        : CompressedPairElement<F, 0>(std::forward<R>(first)), CompressedPairElement<S, 1>(std::forward<U>(second)) {}

    F& GetFirst() noexcept {
        return (static_cast<CompressedPairElement<F, 0>*>(this))->Getts();
    }

    const F& GetFirst() const noexcept {
        return (static_cast<const CompressedPairElement<F, 0>*>(this))->Getts();
    }

    S& GetSecond() noexcept {
        return (static_cast<CompressedPairElement<S, 1>*>(this))->Getts();
    }

    const S& GetSecond() const noexcept {
        return (static_cast<const CompressedPairElement<S, 1>*>(this))->Getts();
    }
};
