#pragma once

constexpr int next_prime(int x) {
    if (x <= 1) {
        return 2;
    }
    for (int i = x;; ++i) {
        bool flag = false;
        for (int j = 2; j * j <= i; ++j) {
            if (i % j == 0) {
                flag = true;
                break;
            }
        }
        if (!flag) {
            return i;
        }
    }
}
