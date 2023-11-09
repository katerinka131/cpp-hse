#pragma once

#include <iostream>
#include <type_traits>

template <typename T, size_t I, bool = std::is_empty_v<T> && !std::is_final_v<T>>
struct CompressedPairElement {
public:
    CompressedPairElement() noexcept(std::is_nothrow_default_constructible_v<T>) : dat_(T()) {
    }

    template <typename U>
    CompressedPairElement(U v) : dat_(std::move(v)) {
    }

    const T& Get() const noexcept {
        return dat_;
    }
    T& Get() noexcept {
        return dat_;
    }

private:
    T dat_;
};

template <typename T, size_t I>
struct CompressedPairElement<T, I, true> : public T {
public:
    CompressedPairElement() noexcept(std::is_nothrow_default_constructible_v<T>) {
    }

    template <typename U>
    CompressedPairElement(U&& v) : T(std::forward<U>(v)) {
    }

    const T& Get() const noexcept {
        return *this;
    }
    T& Get() noexcept {
        return *this;
    }
};

template <typename F, typename S>
class CompressedPair : private CompressedPairElement<F, 0>, private CompressedPairElement<S, 1> {

public:
    CompressedPair() {
    }

    template <typename R, typename U>
    CompressedPair(R&& first, U&& second)
        : CompressedPairElement<F, 0>(std::forward<R>(first)),
          CompressedPairElement<S, 1>(std::move(second)) {
    }

    F& GetFirst() noexcept {
        return (static_cast<CompressedPairElement<F, 0>*>(this))->Get();
    }

    const F& GetFirst() const noexcept {
        return (static_cast<const CompressedPairElement<F, 0>*>(this))->Get();
    }

    S& GetSecond() noexcept {
        return (static_cast<CompressedPairElement<S, 1>*>(this))->Get();
    }

    const S& GetSecond() const noexcept {
        return (static_cast<const CompressedPairElement<S, 1>*>(this))->Get();
    }
};
