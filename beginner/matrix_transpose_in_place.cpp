/**
 * @file matrix_transpose_in_place.cpp
 * @brief Demonstrates in-place transpose for square matrices and out-of-place for rectangular matrices.
 *
 * Concepts:
 * - A transpose swaps the row and column indices: transposed[j][i] = original[i][j].
 * - For a square N x N matrix, the transpose can be performed strictly in-place with O(1)
 *   auxiliary space by swapping elements across the main diagonal:
 *   for i from 0 to N-1:
 *     for j from i+1 to N-1:
 *       swap(mat[i][j], mat[j][i])
 * - For an M x N rectangular matrix, elements are mapped into a new N x M matrix.
 *
 * Time Complexity:
 * - Square In-Place: O(N^2)
 * - Rectangular: O(M * N)
 * Auxiliary Space:
 * - Square In-Place: O(1)
 * - Rectangular: O(M * N) for the transposed matrix
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

/**
 * @brief Transposes an N x N square matrix strictly in-place.
 * @param mat Reference to the square matrix.
 */
void transposeSquareInPlace(std::vector<std::vector<int>>& mat) {
    if (mat.empty() || mat[0].empty()) return;
    int n = static_cast<int>(mat.size());

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            std::swap(mat[i][j], mat[j][i]);
        }
    }
}

/**
 * @brief Transposes an M x N rectangular matrix into an N x M matrix.
 * @param mat Original M x N matrix.
 * @return Transposed N x M matrix.
 */
std::vector<std::vector<int>> transposeRectangular(const std::vector<std::vector<int>>& mat) {
    if (mat.empty() || mat[0].empty()) return {};
    int m = static_cast<int>(mat.size());
    int n = static_cast<int>(mat[0].size());

    std::vector<std::vector<int>> transposed(n, std::vector<int>(m));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            transposed[j][i] = mat[i][j];
        }
    }
    return transposed;
}

/**
 * @brief Pretty prints a 2D matrix with aligned columns.
 * @param mat Matrix to display.
 */
void printMatrix(const std::vector<std::vector<int>>& mat) {
    if (mat.empty() || mat[0].empty()) {
        std::cout << "  (Empty matrix)\n";
        return;
    }
    for (const auto& row : mat) {
        std::cout << "  ";
        for (int val : row) {
            std::cout << std::setw(5) << val;
        }
        std::cout << '\n';
    }
}

int main() {
    std::cout << "=== Matrix Transpose In-Place and Rectangular ===\n\n";

    // 1. Square Matrix In-Place Transpose (4x4)
    std::cout << "1. Square Matrix In-Place Transpose (4 x 4):\n";
    std::vector<std::vector<int>> square = {
        { 1,  2,  3,  4},
        { 5,  6,  7,  8},
        { 9, 10, 11, 12},
        {13, 14, 15, 16}
    };

    std::cout << "Original Matrix:\n";
    printMatrix(square);

    transposeSquareInPlace(square);

    std::cout << "\nTransposed Matrix (In-Place):\n";
    printMatrix(square);
    std::cout << '\n';

    // 2. Rectangular Matrix Transpose (3x5 -> 5x3)
    std::cout << "2. Rectangular Matrix Transpose (3 x 5 -> 5 x 3):\n";
    std::vector<std::vector<int>> rect = {
        {10, 20, 30, 40, 50},
        {60, 70, 80, 90, 100},
        {11, 22, 33, 44, 55}
    };

    std::cout << "Original Rectangular Matrix (3 x 5):\n";
    printMatrix(rect);

    auto transposedRect = transposeRectangular(rect);

    std::cout << "\nTransposed Matrix (5 x 3):\n";
    printMatrix(transposedRect);

    return 0;
}
