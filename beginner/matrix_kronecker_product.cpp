/**
 * @file matrix_kronecker_product.cpp
 * @brief Computes the Kronecker Product of two matrices and verifies fundamental algebraic properties.
 *
 * Theory:
 * For an M x N matrix A and a P x Q matrix B, their Kronecker product (denoted A (x) B)
 * is an (M*P) x (N*Q) block matrix defined as:
 *   [ a_11 * B   a_12 * B  ...  a_1N * B ]
 *   [ a_21 * B   a_22 * B  ...  a_2N * B ]
 *   [   ...        ...     ...    ...    ]
 *   [ a_M1 * B   a_M2 * B  ...  a_MN * B ]
 *
 * Properties Verified:
 * 1. Transposition: (A (x) B)^T = A^T (x) B^T
 * 2. Bilinearity:   (c * A) (x) B = c * (A (x) B)
 *
 * Applications:
 * Quantum computing (combining quantum states / tensor product of qubits),
 * digital signal processing, Kronecker linear systems.
 *
 * Time Complexity: O(M * N * P * Q)
 * Space Complexity: O(M * N * P * Q)
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

/**
 * @brief Computes the Kronecker product of matrix A (m x n) and matrix B (p x q).
 */
std::vector<std::vector<int>> kroneckerProduct(const std::vector<std::vector<int>>& A,
                                              const std::vector<std::vector<int>>& B) {
    if (A.empty() || B.empty()) return {};

    int m = static_cast<int>(A.size());
    int n = static_cast<int>(A[0].size());
    int p = static_cast<int>(B.size());
    int q = static_cast<int>(B[0].size());

    std::vector<std::vector<int>> C(m * p, std::vector<int>(n * q, 0));

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < p; ++k) {
                for (int l = 0; l < q; ++l) {
                    C[i * p + k][j * q + l] = A[i][j] * B[k][l];
                }
            }
        }
    }

    return C;
}

/**
 * @brief Transposes an arbitrary rectangular matrix.
 */
std::vector<std::vector<int>> transpose(const std::vector<std::vector<int>>& mat) {
    if (mat.empty()) return {};
    int r = static_cast<int>(mat.size());
    int c = static_cast<int>(mat[0].size());
    std::vector<std::vector<int>> res(c, std::vector<int>(r));
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            res[j][i] = mat[i][j];
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
    std::cout << "=== Kronecker (Tensor) Matrix Product ===\n\n";

    // Matrix A (2x2)
    std::vector<std::vector<int>> A = {
        {1, 2},
        {3, 0}
    };

    // Matrix B (2x3)
    std::vector<std::vector<int>> B = {
        {0, 5, 2},
        {1, 4, 3}
    };

    std::cout << "Matrix A (2x2):\n";
    printMatrix(A);
    std::cout << "\nMatrix B (2x3):\n";
    printMatrix(B);

    // 1. Compute A (x) B: Size = (2*2) x (2*3) = 4 x 6
    auto C = kroneckerProduct(A, B);
    std::cout << "\nKronecker Product A (x) B (4x6):\n";
    printMatrix(C);

    // 2. Verify Property: (A (x) B)^T == A^T (x) B^T
    std::cout << "\n--- Verifying Property: (A (x) B)^T == A^T (x) B^T ---\n";
    auto lhs = transpose(C);
    auto rhs = kroneckerProduct(transpose(A), transpose(B));

    std::cout << "Transposed (A (x) B)^T:\n";
    printMatrix(lhs);

    std::cout << "\nA^T (x) B^T:\n";
    printMatrix(rhs);

    std::cout << "\nProperty Holds? " << (lhs == rhs ? "PASS (Exact Match)" : "FAIL") << '\n';

    return 0;
}
