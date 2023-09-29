#pragma once

#include <algorithm>

class ListHook {
public:
    ListHook() = default;

    bool IsLinked() const {
        return next_ != this && prev_ != this;
    }

    void Unlink() {
        next_->prev_ = prev_;
        prev_->next_ = next_;
        prev_ = this;
        next_ = this;
    }

    ~ListHook() {
        Unlink();
    }

    ListHook(const ListHook&) = delete;

    virtual void Swap(ListHook& other) {
        std::swap(prev_, other.prev_);
        std::swap(next_, other.next_);
    }

private:
    template <class T>
    friend class List;

    ListHook* prev_ = this;
    ListHook* next_ = this;
};

template <typename T>
class List {
public:
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
    public:
        Iterator(ListHook* h) : current_(h) {}

        Iterator& operator++() {
            current_ = current_->next_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator it = *this;
            current_ = current_->next_;
            return it;
        }

        T& operator*() const {
            return static_cast<T&>(*current_);
        }

        T* operator->() const {
            return static_cast<T*>(current_);
        }

        bool operator==(const Iterator& rhs) const {
            return current_ == rhs.current_;
        }

        bool operator!=(const Iterator& rhs) const {
            return current_ != rhs.current_;
        }

        Iterator& operator=(const Iterator& other) {
            current_ = other.current_;
        }

    private:
        ListHook* current_;
    };

    List() : dummy_(ListHook()) {
        dummy_.next_ = &dummy_;
        dummy_.prev_ = &dummy_;
    }

    List(const List&) = delete;

    List(List&& other) {
        dummy_ = ListHook();
        dummy_.next_ = other.dummy_.next_;
        dummy_.prev_ = other.dummy_.prev_;
        dummy_.next_->prev_ = &dummy_;
        dummy_.prev_->next_ = &dummy_;
        other.dummy_.next_ = &other.dummy_;
        other.dummy_.prev_ = &other.dummy_;
    }

    ~List() {
        while (dummy_.prev_ != &dummy_) {
            dummy_.prev_->Unlink();
        }
    }

    List& operator=(const List&) = delete;
    List& operator=(List&& other) {
        dummy_ = ListHook();
        dummy_.next_ = other.dummy_.next_;
        dummy_.prev_ = other.dummy_.prev_;
        dummy_.next_->prev_ = &dummy_;
        dummy_.prev_->next_ = &dummy_;
        other.dummy_.next_ = &other.dummy_;
        other.dummy_.prev_ = &other.dummy_;
        return *this;
    }

    bool IsEmpty() const {
        return dummy_.next_ == &dummy_;
    }

    size_t Size() const {
        size_t sz = 0;
        ListHook* current = dummy_.next_;
        while (current != &dummy_) {
            ++sz;
            current = current->next_;
        }
        return sz;
    }
    
    void PushBack(T* elem) {
        if (IsEmpty()) {
            dummy_.prev_ = elem;
            dummy_.next_ = elem;
            elem->prev_ = &dummy_;
            elem->next_ = &dummy_;
            return;
        }
        dummy_.prev_->next_ = elem;
        elem->prev_ = dummy_.prev_;
        dummy_.prev_ = elem;
        elem->next_ = &dummy_;
    }

    void PushFront(T* elem) {
        if (IsEmpty()) {
            dummy_.prev_ = elem;
            dummy_.next_ = elem;
            elem->prev_ = &dummy_;
            elem->next_ = &dummy_;
            return;
        }
        dummy_.next_->prev_ = elem;
        elem->next_ = dummy_.next_;
        dummy_.next_ = elem;
        elem->prev_ = &dummy_;
    }

    T& Front() {
        return static_cast<T&>(*dummy_.next_);
    }

    const T& Front() const {
        return static_cast<T&>(*dummy_.next_);
    }

    T& Back() {
        return static_cast<T&>(*dummy_.prev_);
    }

    const T& Back() const {
        return static_cast<T&>(*dummy_.prev_);
    }

    void PopBack() {
        dummy_.prev_->Unlink();
    }

    void PopFront() {
        dummy_.next_->Unlink();
    }

    Iterator Begin() {
        return IteratorTo(static_cast<T*>(dummy_.next_));
    }

    Iterator End() {
        return IteratorTo(static_cast<T*>(&dummy_));
    }

    // complexity of this function must be O(1)
    Iterator IteratorTo(T* element) {
        return Iterator(element);
    }

private:
    ListHook dummy_;
};

template <typename T>
typename List<T>::Iterator begin(List<T>& list) {  // NOLINT
    return list.Begin();
}

template <typename T>
typename List<T>::Iterator end(List<T>& list) {  // NOLINT
    return list.End();
}
