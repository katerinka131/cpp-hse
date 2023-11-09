#pragma once

#include "compressed_pair.h"

#include <algorithm>
#include <memory>

template <typename T>
class Slug {
public:
    Slug() = default;

    template <typename D>
    Slug(const Slug<D>&) {
    }

    void operator()(T* ptr) {
        static_assert(sizeof(T) > 0);
        delete ptr;
    }
};

template <typename T>
class Slug<T[]> {
public:
    Slug() = default;

    template <typename D>
    Slug(const Slug<D>&) {
    }

    void operator()(T* ptr) {
        static_assert(sizeof(T) > 0);
        delete[] ptr;
    }
};

template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
public:
    explicit UniquePtr(T* ptr = nullptr) {
        date_.GetFirst() = ptr;
        date_.GetSecond() = Deleter();
    }
    UniquePtr(T* ptr, const Deleter& deleter) {
        date_.GetFirst() = ptr;
        date_.GetSecond() = deleter;
    }
    template <typename D>
    UniquePtr(T* ptr, D&& deleter) {
        date_.GetSecond() = std::forward<D>(deleter);
        date_.GetFirst() = ptr;
    }

    UniquePtr(UniquePtr&& other) noexcept : date_(other.Release(), std::move(other.GetDeleter())) {
    }

    template <typename Y, typename D>
    UniquePtr(UniquePtr<Y, D>&& other) noexcept
        : date_(other.Release(), std::move(other.GetDeleter())) {
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (&other == this) {
            return *this;
        }
        Reset(other.Release());
        date_.GetSecond() = std::move(other.GetDeleter());
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        Reset();
        return *this;
    }

    ~UniquePtr() noexcept {
        Reset();
    }

    T* Release() noexcept {
        T* old_ptr = date_.GetFirst();
        date_.GetFirst() = nullptr;
        return old_ptr;
    }

    void Reset(T* ptr = nullptr) noexcept {
        if (date_.GetFirst() != ptr) {
            T* old_ptr = date_.GetFirst();
            date_.GetFirst() = ptr;
            if (old_ptr) {
                date_.GetSecond()(old_ptr);
            }
        }
    }

    T* Get() const {
        return date_.GetFirst();
    }
    void Swap(UniquePtr& other) {
        std::swap(date_, other.date_);
    }

    const Deleter& GetDeleter() const {
        return date_.GetSecond();
    }
    Deleter& GetDeleter() {
        return date_.GetSecond();
    }

    operator bool() const {
        return date_.GetFirst() != nullptr;
    }

    std::add_lvalue_reference_t<T> operator*() const {
        return *date_.GetFirst();
    }
    T* operator->() const {
        return date_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> date_;
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    explicit UniquePtr(T* ptr = nullptr) {
        date_.GetFirst() = ptr;
        date_.GetSecond() = Deleter();
    }
    UniquePtr(T* ptr, const Deleter& deleter) {
        date_.GetSecond() = deleter;
        date_.GetFirst() = ptr;
    }
    UniquePtr(T* ptr, Deleter&& deleter) {
        date_.GetSecond() = std::move(deleter);
        date_.GetFirst() = ptr;
    }

    UniquePtr(UniquePtr&& other) noexcept {
        *this = std::move(other);
    }

    template <typename Y, typename D>
    UniquePtr(UniquePtr<Y, D>&& other) noexcept
        : date_(other.Release(), std::move(other.GetDeleter())) {
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            Reset(other.Release());
            date_.GetSecond() = std::move(other.date_.GetSecond());
        }
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) noexcept {
        Reset();
        return *this;
    }

    ~UniquePtr() {
        Reset();
    }

    T* Release() {
        T* old_ptr = date_.GetFirst();
        date_.GetFirst() = nullptr;
        return old_ptr;
    }

    void Reset(T* ptr = nullptr) noexcept {
        if (date_.GetFirst() != ptr) {
            date_.GetSecond()(date_.GetFirst());
            date_.GetFirst() = ptr;
        }
    }
    T* Get() const {
        return date_.GetFirst();
    }
    void Swap(UniquePtr& other) {
        std::swap(date_, other.date_);
    }
    const Deleter& GetDeleter() const {
        return date_.GetSecond();
    }
    Deleter& GetDeleter() {
        return date_.GetSecond();
    }
    operator bool() const {
        return date_.GetFirst() != nullptr;
    }

    T& operator[](size_t ind) const {
        return date_.GetFirst()[ind];
    }

    T* operator->() const {
        return date_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> date_;
};
