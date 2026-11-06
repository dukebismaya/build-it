/**
 * @file matrix_reflection_symmetry.cpp
 * @brief Performs Horizontal, Vertical, and Diagonal Reflections on 2D matrices and verifies involution.
 *
 * Theory:
 * 1. Horizontal Reflection (Flip Rows Upside-Down):
 *      H(A)[i][j] = A[M - 1 - i][j]
 * 2. Vertical Reflection (Flip Columns Left-to-Right):
 *      V(A)[i][j] = A[i][N - 1 - j]
 * 3. Main Diagonal Reflection (Transpose for square matrix):
 *      D(A)[i][j] = A[j][i]
 * 4. Anti-Diagonal Reflection:
 *      AD(A)[i][j] = A[N - 1 - j][N - 1 - i]
 *
 * Algebraic Properties Verified:
 * - Involution: H(H(A)) = A and V(V(A)) = A
 * - Commutativity of orthogonal reflections: H(V(A)) = V(H(A)) = 180-degree rotation!
 *
 * Time Complexity: O(M * N)
 * Space Complexity: O(M * N)
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

/**
 * @brief Reflects a matrix horizontally (flips upside down).
 */
std::vector<std::vector<int>> reflectHorizontal(const std::vector<std::vector<int>>& mat) {
    if (mat.empty()) return {};
    int m = static_cast<int>(mat.size());
    int n = static_cast<int>(mat[0].size());
    std::vector<std::vector<int>> res(m, std::vector<int>(n));

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            res[i][j] = mat[m - 1 - i][j];
        }
    }
    return res;
}

/**
 * @brief Reflects a matrix vertically (flips left-to-right).
 */
std::vector<std::vector<int>> reflectVertical(const std::vector<std::vector<int>>& mat) {
    if (mat.empty()) return {};
    int m = static_cast<int>(mat.size());
    int n = static_cast<int>(mat[0].size());
    std::vector<std::vector<int>> res(m, std::vector<int>(n));

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            res[i][j] = mat[i][n - 1 - j];
        }
    }
    return res;
}

/**
 * @brief Reflects an n x n square matrix across its anti-diagonal.
 */
std::vector<std::vector<int>> reflectAntiDiagonal(const std::vector<std::vector<int>>& mat) {
    if (mat.empty()) return {};
    int n = static_cast<int>(mat.size());
    std::vector<std::vector<int>> res(n, std::vector<int>(n));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            res[i][j] = mat[n - 1 - j][n - 1 - i];
        }
    }
    return res;
}

void printMatrix(const std::vector<std::vector<int>>& mat) {
    for (const auto& row : mat) {
        std::cout << "  ";
        for (int val : row) {
            std::cout << std::setw(5) << val;
        }
        std::cout << '\n';
    }
}

int main() {
    std::cout << "=== Matrix Reflection Symmetries and Involution ===\n\n";

    std::vector<std::vector<int>> original = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    std::cout << "Original Matrix A (3x3):\n";
    printMatrix(original);

    // 1. Horizontal Reflection
    auto horiz = reflectHorizontal(original);
    std::cout << "\nHorizontal Reflection H(A) (Flip Upside-Down):\n";
    printMatrix(horiz);

    // 2. Vertical Reflection
    auto vert = reflectVertical(original);
    std::cout << "\nVertical Reflection V(A) (Flip Left-to-Right):\n";
    printMatrix(vert);

    // 3. Anti-Diagonal Reflection
    auto antiDiag = reflectAntiDiagonal(original);
    std::cout << "\nAnti-Diagonal Reflection AD(A):\n";
    printMatrix(antiDiag);

    // 4. Verify Involution: H(H(A)) == A
    auto horizTwice = reflectHorizontal(horiz);
    std::cout << "\n--- Verifying Involution ---\n";
    std::cout << "H(H(A)) == A: " << (horizTwice == original ? "PASS (Involution Confirmed)" : "FAIL") << '\n';

    auto vertTwice = reflectVertical(vert);
    std::cout << "V(V(A)) == A: " << (vertTwice == original ? "PASS (Involution Confirmed)" : "FAIL") << '\n';

    // 5. Verify H(V(A)) == V(H(A)) (180-degree rotation)
    auto hv = reflectHorizontal(vert);
    auto vh = reflectVertical(horiz);
    std::cout << "H(V(A)) == V(H(A)): " << (hv == vh ? "PASS (Equal to 180-deg rotation)" : "FAIL") << '\n';

    return 0;
}

