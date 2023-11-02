#pragma once

template <unsigned a, unsigned b>
struct Pow {
    static const unsigned value = Pow<a, b - 1>::value * a;
};

template <unsigned a>
struct Pow<a, 0> {
    static const unsigned value = 1;
};
