#pragma once

#include <atomic>
#include <optional>
#include <stdexcept>
#include <utility>

template <class T>
class MPSCStack {
public:
    void Push(const T& value) {
        Node* new_node = new Node(value, data_.load());
        while (!data_.compare_exchange_weak(new_node->next, new_node)) {
            new_node->next = data_.load();
        }
    }

    std::optional<T> Pop() {
        Node* head = data_.load();
        do {
            if (!head) {
                return std::nullopt;
            }
        } while (!data_.compare_exchange_weak(head, head->next));
        T value = head->value;
        delete head;
        return value;
    }

    // DequeuedAll Pop's all elements from the stack and calls cb() for each.
    //
    // Not safe to call concurrently with Pop()
    template <class TFn>
    void DequeueAll(const TFn& cb) {
        Node* head = data_.load();
        while (!data_.compare_exchange_weak(head, nullptr)) {
        }
        while (head) {
            cb(head->value);
            Node* ptr = head;
            head = head->next;
            delete ptr;
        }
    }

    ~MPSCStack() {
        Node* head = data_.load();
        while (head) {
            Node* ptr = head;
            head = head->next;
            delete ptr;
        }
    }

private:
    struct Node {
        T value;
        Node* next;
        Node(const T& x, Node* ptr = nullptr) : value(x), next(ptr) {
        }
    };

    std::atomic<Node*> data_{nullptr};
};
