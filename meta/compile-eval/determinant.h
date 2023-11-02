#pragma once

#include <array>

template <size_t N>
constexpr int determinant(const int a[N][N]) {
    int det = 0;
    if (N == 1) {
        return a[0][0];
    }
    for (int i = 0; i < N; i++) {
        int submatrix[N - 1][N - 1];
        for (int j = 1; j < N; j++) {
            int k = 0;
            for (int l = 0; l < N; l++) {
                if (l != i) {
                    submatrix[j - 1][k] = a[j][l];
                    k++;
                }
            }
        }
        int sign = (i % 2 == 0) ? 1 : -1;
        det += sign * a[0][i] * determinant<N - 1>(submatrix);
    }
    return det;
}

template <size_t N>
constexpr int determinant(const std::array<std::array<int, N>, N>& a) {
    int matrix[N][N];
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = a[i][j];
        }
    }
    return determinant<N>(matrix);
}

template <>
constexpr int determinant<0>(const std::array<std::array<int, 0>, 0>& a) {
    return 1;
}