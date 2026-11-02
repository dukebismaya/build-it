/**
 * @file matrix_inverse_gauss_jordan.cpp
 * @brief Computes Matrix Inversion using Gauss-Jordan Elimination with Partial Pivoting.
 *
 * Theory:
 * An n x n square matrix A has an inverse A^-1 if and only if det(A) != 0.
 *
 * Gauss-Jordan Elimination:
 * 1. Form the augmented matrix [ A | I ] of dimension n x 2n, where I is the n x n identity matrix.
 * 2. Perform elementary row operations with partial pivoting (to minimize numerical errors)
 *    to transform the left half into the identity matrix I:
 *      [ A | I ]  --->  [ I | A^-1 ]
 * 3. The right half is the inverse matrix A^-1.
 * 4. If any pivot is approximately zero (abs(pivot) < EPS), the matrix is singular (non-invertible).
 *
 * Time Complexity: O(N^3)
 * Space Complexity: O(N^2)
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

struct InversionResult {
    bool isSingular;
    std::vector<std::vector<double>> inverse;
};

/**
 * @brief Inverts an n x n matrix using Gauss-Jordan elimination with partial pivoting.
 */
InversionResult invertMatrixGaussJordan(std::vector<std::vector<double>> A) {
    int n = static_cast<int>(A.size());
    const double EPS = 1e-9;

    // Augmented matrix [ A | I ] of size n x 2n
    std::vector<std::vector<double>> aug(n, std::vector<double>(2 * n, 0.0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            aug[i][j] = A[i][j];
        }
        aug[i][n + i] = 1.0; // Identity matrix on the right
    }

    // Forward and backward elimination
    for (int i = 0; i < n; ++i) {
        // Partial pivoting: find maximum element in column i
        int pivotRow = i;
        for (int r = i + 1; r < n; ++r) {
            if (std::abs(aug[r][i]) > std::abs(aug[pivotRow][i])) {
                pivotRow = r;
            }
        }

        if (std::abs(aug[pivotRow][i]) < EPS) {
            return {true, {}}; // Singular matrix
        }

        if (pivotRow != i) {
            std::swap(aug[i], aug[pivotRow]);
        }

        // Normalize pivot row
        double pivotVal = aug[i][i];
        for (int j = 0; j < 2 * n; ++j) {
            aug[i][j] /= pivotVal;
        }

        // Eliminate column i in all other rows
        for (int r = 0; r < n; ++r) {
            if (r != i) {
                double factor = aug[r][i];
                for (int j = 0; j < 2 * n; ++j) {
                    aug[r][j] -= factor * aug[i][j];
                }
            }
        }
    }

    // Extract inverse matrix from the right half
    std::vector<std::vector<double>> inv(n, std::vector<double>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            inv[i][j] = aug[i][n + j];
        }
    }

    return {false, inv};
}

/**
 * @brief Multiplies two square matrices to verify A * A^-1 == I.
 */
std::vector<std::vector<double>> multiplySquare(const std::vector<std::vector<double>>& A,
                                               const std::vector<std::vector<double>>& B) {
    int n = static_cast<int>(A.size());
    std::vector<std::vector<double>> C(n, std::vector<double>(n, 0.0));
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k) {
            for (int j = 0; j < n; ++j) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

void printMatrix(const std::vector<std::vector<double>>& mat) {
    for (const auto& row : mat) {
        std::cout << "  ";
        for (double val : row) {
            if (std::abs(val) < 1e-12) val = 0.0;
            std::cout << std::fixed << std::setprecision(4) << std::setw(9) << val;
        }
        std::cout << '\n';
    }
}

int main() {
    std::cout << "=== Gauss-Jordan Matrix Inversion with Partial Pivoting ===\n\n";

    // Example 1: 3x3 Invertible Matrix
    std::vector<std::vector<double>> A1 = {
        {2.0, 1.0, 1.0},
        {1.0, 3.0, 2.0},
        {1.0, 0.0, 0.0}
    };

    std::cout << "Matrix A1 (3x3):\n";
    printMatrix(A1);

    auto res1 = invertMatrixGaussJordan(A1);
    if (!res1.isSingular) {
        std::cout << "\nInverse Matrix A1^-1:\n";
        printMatrix(res1.inverse);

        auto checkIdentity = multiplySquare(A1, res1.inverse);
        std::cout << "\nVerification Product (A1 * A1^-1):\n";
        printMatrix(checkIdentity);
    } else {
        std::cout << "Matrix is singular!\n";
    }
    std::cout << '\n';

    // Example 2: Singular Matrix (Determinant = 0)
    std::vector<std::vector<double>> A2 = {
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0},
        {7.0, 8.0, 9.0}
    };

    std::cout << "Matrix A2 (3x3 Singular):\n";
    printMatrix(A2);

    auto res2 = invertMatrixGaussJordan(A2);
    std::cout << "Inversion Status: " 
              << (res2.isSingular ? "CORRECTLY DETECTED AS SINGULAR (Non-Invertible)" : "Invertible") << '\n';

    return 0;
}
