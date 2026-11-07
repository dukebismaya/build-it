/**
 * @file matrix_circulant_generator.cpp
 * @brief Generation and algebraic verification of Circulant Matrices in C++17.
 *
 * A circulant matrix is an N x N square matrix where each row is a circular right shift
 * (or left shift) of the previous row. A standard right-shift circulant matrix C has the form:
 *
 *   [ c_0   c_1   c_2  ...  c_{n-1} ]
 *   [ c_{n-1} c_0 c_1  ...  c_{n-2} ]
 *   [ c_{n-2} c_{n-1} c_0 ... c_{n-3} ]
 *   [ ...                           ]
 *   [ c_1   c_2   c_3  ...  c_0     ]
 *
 * Circulant matrices play an essential role in:
 *   - Digital Signal Processing (discrete cyclic convolutions).
 *   - Fast Fourier Transforms (diagonalized by the DFT matrix).
 *   - Numerical linear algebra and cyclic graph adjacency matrices.
 *
 * Properties implemented and verified:
 *   - Construction from first row vector in O(N^2).
 *   - Cyclic indexing property: C[i][j] = c[(j - i + N) % N].
 *   - Symmetry check (is circulant also symmetric/Toeplitz).
 *   - Trace computation: Tr(C) = N * c_0.
 *
 * Complexity:
 *   - Generation: O(N^2) time, O(N^2) space.
 *   - Verification: O(N^2) time, O(1) auxiliary space.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <numeric>

namespace linalg {

/**
 * @brief Generates an N x N right-circulant matrix from a generating first-row vector.
 * @tparam T Numeric type of matrix entries.
 * @param firstRow The vector [c_0, c_1, ..., c_{n-1}].
 * @return 2D vector representing the N x N circulant matrix.
 */
template <typename T>
std::vector<std::vector<T>> generateCirculantMatrix(const std::vector<T>& firstRow) {
    size_t n = firstRow.size();
    std::vector<std::vector<T>> matrix(n, std::vector<T>(n));

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            // Right-circulant mapping: C[i][j] = firstRow[(j - i + n) % n]
            size_t idx = (j + n - (i % n)) % n;
            matrix[i][j] = firstRow[idx];
        }
    }
    return matrix;
}

/**
 * @brief Verifies whether a given square matrix is right-circulant.
 * @tparam T Numeric type of matrix entries.
 * @param matrix The N x N matrix to verify.
 * @return True if matrix is circulant, false otherwise.
 */
template <typename T>
bool isCirculant(const std::vector<std::vector<T>>& matrix) {
    size_t n = matrix.size();
    if (n == 0) return true;
    for (size_t i = 0; i < n; ++i) {
        if (matrix[i].size() != n) return false;
    }

    for (size_t i = 1; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            size_t prevJ = (j + n - 1) % n;
            if (matrix[i][j] != matrix[i - 1][prevJ]) {
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief Checks if a matrix is symmetric (C == C^T).
 */
template <typename T>
bool isSymmetric(const std::vector<std::vector<T>>& matrix) {
    size_t n = matrix.size();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (matrix[i][j] != matrix[j][i]) return false;
        }
    }
    return true;
}

/**
 * @brief Computes the trace of a square matrix.
 */
template <typename T>
T computeTrace(const std::vector<std::vector<T>>& matrix) {
    T trace = T();
    for (size_t i = 0; i < matrix.size(); ++i) {
        trace += matrix[i][i];
    }
    return trace;
}

/**
 * @brief Prints a 2D matrix formatted with alignment.
 */
template <typename T>
void printMatrix(const std::vector<std::vector<T>>& matrix, const std::string& label) {
    std::cout << label << " (" << matrix.size() << "x" << matrix.size() << "):\n";
    for (const auto& row : matrix) {
        std::cout << "  [";
        for (size_t j = 0; j < row.size(); ++j) {
            std::cout << std::setw(5) << row[j] << (j + 1 == row.size() ? "" : " ");
        }
        std::cout << " ]\n";
    }
    std::cout << "\n";
}

} // namespace linalg

int main() {
    std::cout << "========================================\n";
    std::cout << "      CIRCULANT MATRIX GENERATOR        \n";
    std::cout << "========================================\n\n";

    // Example 1: 4x4 Circulant Matrix
    std::vector<int> row1 = {1, 2, 3, 4};
    std::cout << "Input row vector: [1, 2, 3, 4]\n";
    auto circ1 = linalg::generateCirculantMatrix(row1);
    linalg::printMatrix(circ1, "Circulant Matrix C_1");

    std::cout << "Verification & Properties of C_1:\n";
    std::cout << "  - Is Circulant? " << (linalg::isCirculant(circ1) ? "YES" : "NO") << "\n";
    std::cout << "  - Is Symmetric? " << (linalg::isSymmetric(circ1) ? "YES" : "NO") << "\n";
    std::cout << "  - Trace: " << linalg::computeTrace(circ1) << " (Expected: 4 * 1 = 4)\n\n";

    // Example 2: Symmetric Circulant Matrix (e.g., [1, 2, 3, 2])
    std::vector<int> row2 = {5, 2, 8, 2};
    std::cout << "Input row vector: [5, 2, 8, 2]\n";
    auto circ2 = linalg::generateCirculantMatrix(row2);
    linalg::printMatrix(circ2, "Circulant Matrix C_2");

    std::cout << "Verification & Properties of C_2:\n";
    std::cout << "  - Is Circulant? " << (linalg::isCirculant(circ2) ? "YES" : "NO") << "\n";
    std::cout << "  - Is Symmetric? " << (linalg::isSymmetric(circ2) ? "YES" : "NO") << "\n";
    std::cout << "  - Trace: " << linalg::computeTrace(circ2) << " (Expected: 4 * 5 = 20)\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
