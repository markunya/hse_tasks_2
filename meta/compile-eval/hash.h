#pragma once

constexpr unsigned long hash(const char *s, unsigned long p, unsigned long mod) {
    return (*s == 0) ? 0 : (*s + (p * hash(s + 1, p, mod)) % mod) % mod;
}
