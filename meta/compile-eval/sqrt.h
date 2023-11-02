template <bool condition, unsigned long N, unsigned long left, unsigned long right>
struct Conditional;

template <unsigned long N, unsigned long curr>
struct Less {
    static const bool value = (curr == 0) || (curr < (N + curr - 1) / curr);
};

template <unsigned long N, unsigned long left, unsigned long right>
struct BinSearch {
    static const unsigned long value =
        Conditional<Less<N, (left + right) / 2>::value, N, left, right>::value;
};

template <unsigned long N, unsigned long x>
struct BinSearch<N, x, x> {
    static const unsigned long value = x;
};

template <bool condition, unsigned long N, unsigned long left, unsigned long right>
struct Conditional {
    static const unsigned long value = BinSearch<N, (left + right) / 2 + 1, right>::value;
};

template <unsigned long N, unsigned long left, unsigned long right>
struct Conditional<false, N, left, right> {
    static const unsigned long value = BinSearch<N, left, (left + right) / 2>::value;
};

template <unsigned long N>
struct Sqrt {
    static const unsigned long value = BinSearch<N, 0, N>::value;
};