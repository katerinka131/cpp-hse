#pragma once
#include <utility>
#include <memory>
#include <algorithm>
#include <typeinfo>

class Any {
public:
    bool _empty;

    Any() : _inner(new Inner<int>(0)) {
        _empty = true;
    }  // default construct to an integer

    Any(const Any& src) : _inner(src._inner->Clone()) {
        _empty = src._empty;
    }  // copy constructor calls clone method of concrete type

    template <typename _Ty>
    Any(_Ty src) : _inner(new Inner<_Ty>(std::forward<_Ty>(src))) {
        _empty = false;
    }

    template <typename _Ty>
    Any& operator=(_Ty src) {  // assign to a concrete type
        _inner = std::make_unique<Inner<_Ty>>(std::forward<_Ty>(src));
        _empty = false;
        return *this;
    }

    Any& operator=(const Any& src) {  // assign to another var type
        _empty = src._empty;
        Any o_tmp(src);
        std::swap(o_tmp._inner, this->_inner);
        return *this;
    }

    ~Any() {
    }

    bool Empty() const {
        return _empty;
    }

    void Clear() {
        *this = Any();
    }

    void Swap(Any& rhs) {
        std::swap(rhs._empty, this->_empty);
        std::swap(rhs._inner, this->_inner);
    }

    template <class T>
    const T& GetValue() const {
        try {
            return *dynamic_cast<Inner<T>&>(*_inner);
        } catch (...) {
            throw std::bad_cast();
        }
    }

    template <class T>
    T& GetValue() {
        try {
            return *dynamic_cast<Inner<T>&>(*_inner);
        } catch (...) {
            throw std::bad_cast();
        }
    }

    struct InnerBase {
        using Pointer = std::unique_ptr<InnerBase>;
        virtual ~InnerBase() {
        }
        virtual InnerBase* Clone() const = 0;
        virtual const std::type_info& Type() const = 0;
    };

    template <typename _Ty>
    struct Inner : InnerBase {
        Inner(_Ty newval) : value_(std::move(newval)) {
        }
        virtual InnerBase* Clone() const override {
            return new Inner(value_);
        }
        virtual const std::type_info& Type() const override {
            return typeid(_Ty);
        }
        _Ty& operator*() {
            return value_;
        }
        const _Ty& operator*() const {
            return value_;
        }

    private:
        _Ty value_;
    };

    InnerBase::Pointer _inner;
};
