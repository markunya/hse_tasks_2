#pragma once

#include <type_traits>
#include <cstdint>
#include <utility>

template<typename T, bool is_first, bool can_use_ebo = std::is_empty_v<T> && !std::is_final_v<T>>
struct CompressedPairMember {
    CompressedPairMember() = default;

    CompressedPairMember(const T& obj) : obj(obj) {
    }

    CompressedPairMember(T&& obj) : obj(std::move(obj)) {
    }

    T& GetObject() {
        return obj;
    }

    const T& GetObject() const {
        return obj;
    }

    T obj;
};

template<typename T, bool is_first>
struct CompressedPairMember<T, is_first, true> : public T {
    CompressedPairMember() = default;

    CompressedPairMember(const T& obj) {
    }

    CompressedPairMember(T&& obj) {
    }

    T& GetObject() {
        return *this;
    }

    const T& GetObject() const {
        return *this;
    }
};


// Me think, why waste time write lot code, when few code do trick.
template <typename F, typename S>
class CompressedPair : public CompressedPairMember<F, true>, public CompressedPairMember<S, false> {
public:
    CompressedPair() = default;

    template <typename First, typename Second>
    CompressedPair(First&& first, Second&& second)
        : CompressedPairMember<F, true>(std::forward<First>(first)),
          CompressedPairMember<S, false>(std::forward<Second>(second)) {
    }

    F& GetFirst() {
        return CompressedPairMember<F, true>::GetObject();
    }

    const F& GetFirst() const {
        return CompressedPairMember<F, true>::GetObject();
    }

    S& GetSecond() {
        return CompressedPairMember<S, false>::GetObject();
    }

    const S& GetSecond() const {
        return CompressedPairMember<S, false>::GetObject();
    }
};
