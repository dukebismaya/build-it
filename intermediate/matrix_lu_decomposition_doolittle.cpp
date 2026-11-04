/**
 * @file matrix_lu_decomposition_doolittle.cpp
 * @brief Performs LU Decomposition (A = L * U) via Doolittle's algorithm and solves linear systems.
 *
 * Theory:
 * Doolittle's algorithm decomposes an n x n square matrix A into:
 *   A = L * U
 * where:
 *   L is a unit lower triangular matrix (diagonal elements L[i][i] = 1, L[i][j] = 0 for j > i)
 *   U is an upper triangular matrix (U[i][j] = 0 for i > j)
 *
 * Formulas:
 * For each step i = 0 to n - 1:
 *   1. Compute row i of U:
 *        U[i][j] = A[i][j] - sum_{k=0}^{i-1} L[i][k] * U[k][j]   (for j = i to n-1)
 *   2. Compute column i of L:
 *        L[j][i] = (A[j][i] - sum_{k=0}^{i-1} L[j][k] * U[k][i]) / U[i][i]   (for j = i+1 to n-1)
 *
 * Solving Linear System A * x = b:
 *   1. Forward substitution:  L * y = b  -> solve for y
 *   2. Backward substitution: U * x = y  -> solve for x
 *
 * Time Complexity:
 * - Factorization: O(N^3)
 * - System Solve:  O(N^2)
 * Space Complexity: O(N^2)
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

struct LUResult {
    bool success;
    std::vector<std::vector<double>> L;
    std::vector<std::vector<double>> U;
};

class DoolittleLU {
public:
    static constexpr double EPS = 1e-9;

    /**
     * @brief Decomposes A into L and U such that A = L * U.
     */
    static LUResult decompose(const std::vector<std::vector<double>>& A) {
        int n = static_cast<int>(A.size());
        std::vector<std::vector<double>> L(n, std::vector<double>(n, 0.0));
        std::vector<std::vector<double>> U(n, std::vector<double>(n, 0.0));

        for (int i = 0; i < n; ++i) {
            L[i][i] = 1.0; // Unit diagonal for L
        }

        for (int i = 0; i < n; ++i) {
            // 1. Upper Triangular U
            for (int j = i; j < n; ++j) {
                double sum = 0.0;
                for (int k = 0; k < i; ++k) {
                    sum += L[i][k] * U[k][j];
                }
                U[i][j] = A[i][j] - sum;
            }

            // Check for singular pivot
            if (std::abs(U[i][i]) < EPS) {
                return {false, {}, {}};
            }

            // 2. Lower Triangular L
            for (int j = i + 1; j < n; ++j) {
                double sum = 0.0;
                for (int k = 0; k < i; ++k) {
                    sum += L[j][k] * U[k][i];
                }
                L[j][i] = (A[j][i] - sum) / U[i][i];
            }
        }

        return {true, L, U};
    }

    /**
     * @brief Solves A * x = b using precomputed L and U factors.
     */
    static std::vector<double> solve(
        const std::vector<std::vector<double>>& L,
        const std::vector<std::vector<double>>& U,
        const std::vector<double>& b) {
        int n = static_cast<int>(L.size());

        // 1. Forward substitution: L * y = b
        std::vector<double> y(n, 0.0);
        for (int i = 0; i < n; ++i) {
            double sum = 0.0;
            for (int k = 0; k < i; ++k) {
                sum += L[i][k] * y[k];
            }
            y[i] = b[i] - sum; // L[i][i] == 1
        }

        // 2. Backward substitution: U * x = y
        std::vector<double> x(n, 0.0);
        for (int i = n - 1; i >= 0; --i) {
            double sum = 0.0;
            for (int k = i + 1; k < n; ++k) {
                sum += U[i][k] * x[k];
            }
            x[i] = (y[i] - sum) / U[i][i];
        }

        return x;
    }
};

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
    std::cout << "=== Doolittle Algorithm (LU Decomposition & Solver) ===\n\n";

    // 3x3 System of equations:
    //  2x +  1y +  1z =  8
    //  4x +  3y +  3z = 18
    //  8x +  7y +  9z = 44
    // Solution: x = 1, y = 2, z = 4
    std::vector<std::vector<double>> A = {
        {2.0, 1.0, 1.0},
        {4.0, 3.0, 3.0},
        {8.0, 7.0, 9.0}
    };
    std::vector<double> b = {8.0, 18.0, 44.0};

    std::cout << "Matrix A (3x3):\n";
    printMatrix(A);

    auto lu = DoolittleLU::decompose(A);
    if (lu.success) {
        std::cout << "\nUnit Lower Triangular Matrix L:\n";
        printMatrix(lu.L);

        std::cout << "\nUpper Triangular Matrix U:\n";
        printMatrix(lu.U);

        auto x = DoolittleLU::solve(lu.L, lu.U, b);
        std::cout << "\nRight-hand Side Vector b: [ ";
        for (double val : b) std::cout << val << " ";
        std::cout << "]\n";

        std::cout << "Computed Solution Vector x: [ ";
        for (double val : x) std::cout << std::fixed << std::setprecision(4) << val << " ";
        std::cout << "]\n";
        std::cout << "Expected Solution:          [ 1.0000 2.0000 4.0000 ]\n";
    } else {
        std::cout << "LU Decomposition failed: matrix has zero pivot.\n";
    }

    return 0;
}
