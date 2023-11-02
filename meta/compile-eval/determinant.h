#pragma once

#include <array>

template <int N>
constexpr int determinant(const int a[N][N]) {
    int det = 0;
    if constexpr (N == 0) {
        return 1;
    } else if constexpr (N == 1) {
        return a[0][0];
    } else {
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
    }
    return det;
}

template <int N>
constexpr int determinant(const std::array<std::array<int, N>, N>& a) {
    int matrix[N][N];
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = a[i][j];
        }
    }
    return determinant<N>(matrix);
}