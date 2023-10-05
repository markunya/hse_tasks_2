#pragma once

template <class Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end) : begin_(begin), end_(end) {
    }

    Iterator begin() const {  // NOLINT
        return begin_;
    }

    Iterator end() const {  // NOLINT
        return end_;
    }

private:
    Iterator begin_, end_;
};

template <typename T>
class RangeIterator {
public:
    RangeIterator() : x_(0), step_(1) {
    }

    RangeIterator(const T& x, const T& step = 1) : x_(x), step_(step) {
    }

    RangeIterator operator++() {
        x_ += step_;
        return *this;
    }

    T operator*() {
        return x_;
    }

    bool operator!=(const RangeIterator& other) {
        return ((x_ + step_ - 1) / step_) != ((other.x_ + other.step_ - 1) / other.step_);
    }

private:
    T x_;
    T step_;
};

template <typename T>
auto Range(T from, T to, T step) {
    return IteratorRange<RangeIterator<T>>(RangeIterator<T>(from, step),
                                           RangeIterator<T>(to, step));
}

template <typename T>
auto Range(T from, T to) {
    return IteratorRange<RangeIterator<T>>(RangeIterator<T>(from), RangeIterator<T>(to));
}

template <typename T>
auto Range(T to) {
    return IteratorRange<RangeIterator<T>>(RangeIterator<T>(), RangeIterator<T>(to));
}

template <typename F, typename S>
class ZipIterator {
public:
    ZipIterator(const F& first, const S& second) : first_(first), second_(second) {
    }

    auto operator*() {
        return std::make_pair(*first_, *second_);
    }

    ZipIterator operator++() {
        ++first_;
        ++second_;
        return *this;
    }

    bool operator!=(const ZipIterator& other) {
        return (first_ != other.first_) && (second_ != other.second_);
    }

private:
    F first_;
    S second_;
};

template <typename T, typename U>
auto Zip(const T& first, const U& second) {
    return IteratorRange(ZipIterator(first.begin(), second.begin()),
                         ZipIterator(first.end(), second.end()));
}

template <typename T>
class GroupIterator {
public:
    GroupIterator(const IteratorRange<T>& x, const T& end) : x_(x), end_(end) {
    }

    GroupIterator operator++() {
        auto it = x_.end();
        while (it != end_ && *it == *x_.end()) {
            ++it;
        }
        x_ = IteratorRange(x_.end(), it);
        return *this;
    }

    bool operator!=(const GroupIterator& other) {
        return x_.begin() != other.x_.begin();
    }

    auto operator*() {
        return x_;
    }

private:
    IteratorRange<T> x_;
    T end_;
};

template <typename T>
auto Group(const T& sequence) {
    auto it = sequence.begin();
    while (it != sequence.end() && *it == *sequence.begin()) {
        ++it;
    }
    return IteratorRange(
        GroupIterator(IteratorRange(sequence.begin(), it), sequence.end()),
        GroupIterator(IteratorRange(sequence.end(), sequence.end()), sequence.end()));
}
