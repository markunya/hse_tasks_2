#pragma once

template <class T>
class ImmutableVector {
public:
    ImmutableVector() : root_(std::make_shared<Node>()) {
    }

    explicit ImmutableVector(size_t count, const T& value = T()) : root_(std::make_shared<Node>()) {
        size_ = count;
        for (size_t i = 0; i < count; ++i) {
            Add(i, Depth(), value, root_.get());
        }
    }

    template <typename Iterator>
    ImmutableVector(Iterator first, Iterator last) : root_(std::make_shared<Node>()) {
        Iterator current = first;
        while (current != last) {
            ++current;
            ++size_;
        }
        size_t i = 0;
        current = first;
        while (current != last) {
            Add(i, Depth(), *current, root_.get());
            ++current;
            ++i;
        }
    }

    ImmutableVector(std::initializer_list<T> l) : root_(std::make_shared<Node>()) {
        size_ = l.size();
        for (size_t i = 0; i < l.size(); ++i) {
            Add(i, Depth(), *(l.begin() + i), root_.get());
        }
    }

    ImmutableVector Set(size_t index, const T& value) {
        return ImmutableVector(SetBuilder(index, Depth(), value, root_.get()), size_);
    }

    const T& Get(size_t index) const {
        size_t d = Depth();
        Node* current = root_.get();
        while (d > 0) {
            size_t j = ((index >> ((d - 1) * kAm)) & (~((~0) << kAm)));
            if (!current->next[j]) {
                current->next[j] = std::make_shared<Node>();
            }
            current = current->next[j].get();
            --d;
        }
        return current->value;
    }

    ImmutableVector PushBack(const T& value) {

        if (size_ == (1 << (Depth() * kAm))) {
            for (size_t i = 0; i < size_; ++i) {
                Add(i, Depth() + 1, Get(i), root_.get());
            }
            if (value == 1024) {
                int x = 1 + 1;
            }
            Add(size_, Depth() + 1, value, root_.get());
            return ImmutableVector<T>(root_, size_ + 1);
        }
        return ImmutableVector(SetBuilder(size_, Depth(), value, root_.get()), size_ + 1);
    }

    ImmutableVector PopBack() {
        return ImmutableVector(root_, size_ - 1);
    }

    size_t Size() const {
        return size_;
    }

private:
    static const size_t kAm = 5;
    static const size_t kCf = (1 << kAm);

    struct Node {
        Node() : next(kCf, nullptr) {
        }
        Node(const T& value) : value(value), next(kCf, nullptr) {
        }
        T value;
        std::vector<std::shared_ptr<Node>> next;
    };

    size_t Depth() const {
        size_t result = 1;
        size_t x = size_;
        while (x > kCf) {
            x = (x + kCf - 1) / kCf;
            ++result;
        }
        return result;
    }

    void Add(size_t index, size_t d, const T& val, Node* current) {
        while (d > 0) {
            size_t j = ((index >> ((d - 1) * kAm)) & (~((~0) << kAm)));
            if (!current->next[j]) {
                current->next[j] = std::make_shared<Node>();
            }
            current = current->next[j].get();
            --d;
        }
        if (d == 0) {
            current->value = val;
        }
    }

    std::shared_ptr<Node> SetBuilder(size_t index, size_t d, const T& val, Node* current) {
        if (d == 0) {
            return std::make_shared<Node>(val);
        }
        size_t j = ((index >> ((d - 1) * kAm)) & (~((~0) << kAm)));
        if (!current->next[j]) {
            current->next[j] = std::make_shared<Node>();
        }
        std::shared_ptr<Node> new_node = std::make_shared<Node>(current->value);
        for (size_t i = 0; i < kCf; ++i) {
            new_node->next[i] = current->next[i];
        }
        new_node->next[j] = SetBuilder(index, d - 1, val, current->next[j].get());
        return new_node;
    }

    ImmutableVector(std::shared_ptr<Node> root, size_t size) : size_(size), root_(root) {
    }

    std::shared_ptr<Node> root_;
    size_t size_ = 0;
};
