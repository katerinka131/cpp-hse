#pragma once
#include <memory>
#include <stdexcept>
template <class T>
class ImmutableVector {
public:
    ImmutableVector() = default;

    explicit ImmutableVector(size_t count, const T& value = T()) {
        for (size_t i = 0; i < count; i++) {
            *this = this->PushBack(value);
        }
    }

    template <typename Iterator>
    ImmutableVector(Iterator first, Iterator last) {
        for (Iterator i = first; i != last; i++) {
            *this = this->PushBack(*i);
        }
    }

    ImmutableVector(std::initializer_list<T> l) : ImmutableVector(l.begin(), l.end()) {
    }

    ImmutableVector Set(size_t index, const T& value) {
        ImmutableVector new_vector = *this;
        new_vector.root_ = Set(root_, index, value);
        return new_vector;
    }

    const T& Get(size_t index) const {
        return Get(root_, index);
    }

    ImmutableVector PushBack(const T& value) {
        ImmutableVector new_vector = *this;
        new_vector.root_ = Set(root_, size_, value);
        new_vector.size_++;
        return new_vector;
    }

    ImmutableVector PopBack() {
        ImmutableVector new_vector = *this;
        new_vector.size_--;
        return new_vector;
    }

    size_t Size() const {
        return size_;
    }

private:
    struct Node {
        std::shared_ptr<Node> children[32];
        T value;
        Node() = default;
        Node(const Node& other) = default;
        bool is_value = false;
    };

    std::shared_ptr<Node> root_ = std::make_shared<Node>();
    size_t size_ = 0;
    static constexpr size_t kBranchingFactor = 32;
    const T& Get(const std::shared_ptr<Node>& node, size_t index) const {
        if (index == 0) {
            return node->value;
        }
        size_t next_index, child_index;
        next_index = index / kBranchingFactor;
        child_index = index % kBranchingFactor;
        return Get(node->children[child_index], next_index);
    }

    std::shared_ptr<Node> Set(const std::shared_ptr<Node>& node, size_t index,
                              const T& value) const {
        std::shared_ptr<Node> new_node = std::make_shared<Node>(*node);
        if (index != 0) {
            size_t child_index, next_index;
            next_index = index / kBranchingFactor;
            child_index = index % kBranchingFactor;

            if (!new_node->children[child_index]) {
                new_node->children[child_index] = std::make_shared<Node>();
            }
            new_node->children[child_index] =
                Set(new_node->children[child_index], next_index, value);
            return new_node;
        } else {
            new_node->value = value;
            new_node->is_value = true;
            return new_node;
        }
    }
};
