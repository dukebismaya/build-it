/**
 * @file cholesky_decomposition_solver.cpp
 * @brief Computes Cholesky Decomposition (A = L * L^T) and solves linear systems A * x = b.
 *
 * Theory:
 * - A real symmetric matrix A is positive-definite (SPD) if x^T A x > 0 for all non-zero vectors x.
 * - The Cholesky decomposition factors A into A = L * L^T, where L is a unique lower triangular
 *   matrix with strictly positive diagonal elements.
 * - Cholesky factorization requires roughly half the operations of standard LU decomposition:
 *   about N^3 / 3 flops.
 *
 * Formulas (Cholesky-Banachiewicz algorithm):
 *   L[j][j] = sqrt(A[j][j] - sum_{k=0}^{j-1} L[j][k]^2)
 *   L[i][j] = (A[i][j] - sum_{k=0}^{j-1} L[i][k] * L[j][k]) / L[j][j]   (for i > j)
 *
 * Solving Linear System A * x = b:
 * 1. Forward substitution:  L * y = b       -> solve for y
 * 2. Back substitution:     L^T * x = y     -> solve for x
 *
 * Time Complexity:
 * - Factorization: O(N^3)
 * - Linear Solve: O(N^2)
 * Space Complexity: O(N^2)
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

struct CholeskyResult {
    bool isPositiveDefinite;
    std::vector<std::vector<double>> L;
};

/**
 * @brief Performs Cholesky Decomposition on a symmetric square matrix.
 * @param A Symmetric n x n matrix.
 * @return CholeskyResult containing success flag and lower triangular matrix L.
 */
CholeskyResult choleskyDecompose(const std::vector<std::vector<double>>& A) {
    int n = static_cast<int>(A.size());
    std::vector<std::vector<double>> L(n, std::vector<double>(n, 0.0));

    const double EPS = 1e-9;

    for (int j = 0; j < n; ++j) {
        double diagSum = 0.0;
        for (int k = 0; k < j; ++k) {
            diagSum += L[j][k] * L[j][k];
        }

        double val = A[j][j] - diagSum;
        if (val <= EPS) {
            // Not positive-definite
            return {false, {}};
        }

        L[j][j] = std::sqrt(val);

        for (int i = j + 1; i < n; ++i) {
            double offDiagSum = 0.0;
            for (int k = 0; k < j; ++k) {
                offDiagSum += L[i][k] * L[j][k];
            }
            L[i][j] = (A[i][j] - offDiagSum) / L[j][j];
        }
    }

    return {true, L};
}

/**
 * @brief Solves A * x = b given Cholesky factor L (where A = L * L^T).
 * @param L Lower triangular matrix from Cholesky decomposition.
 * @param b Right-hand side vector.
 * @return Solution vector x.
 */
std::vector<double> choleskySolve(const std::vector<std::vector<double>>& L,
                                  const std::vector<double>& b) {
    int n = static_cast<int>(L.size());

    // 1. Forward substitution: L * y = b
    std::vector<double> y(n, 0.0);
    for (int i = 0; i < n; ++i) {
        double sum = 0.0;
        for (int k = 0; k < i; ++k) {
            sum += L[i][k] * y[k];
        }
        y[i] = (b[i] - sum) / L[i][i];
    }

    // 2. Backward substitution: L^T * x = y
    std::vector<double> x(n, 0.0);
    for (int i = n - 1; i >= 0; --i) {
        double sum = 0.0;
        for (int k = i + 1; k < n; ++k) {
            sum += L[k][i] * x[k]; // Note: (L^T)[i][k] == L[k][i]
        }
        x[i] = (y[i] - sum) / L[i][i];
    }

    return x;
}

void printMatrix(const std::vector<std::vector<double>>& mat) {
    for (const auto& row : mat) {
        std::cout << "  ";
        for (double val : row) {
            std::cout << std::fixed << std::setprecision(4) << std::setw(9) << val;
        }
        std::cout << '\n';
    }
}

int main() {
    std::cout << "=== Cholesky Decomposition & Linear System Solver ===\n\n";

    // Example 1: 3x3 Symmetric Positive Definite Matrix
    // A = [ 4  12 -16 ]
    //     [ 12  37 -43 ]
    //     [-16 -43  98 ]
    std::vector<std::vector<double>> A1 = {
        {  4.0,  12.0, -16.0},
        { 12.0,  37.0, -43.0},
        {-16.0, -43.0,  98.0}
    };
    std::vector<double> b1 = {4.0, 19.0, -29.0};

    std::cout << "System 1 Matrix A (3x3 SPD):\n";
    printMatrix(A1);

    auto res1 = choleskyDecompose(A1);
    if (res1.isPositiveDefinite) {
        std::cout << "\nCholesky Factor L (Lower Triangular):\n";
        printMatrix(res1.L);

        auto x1 = choleskySolve(res1.L, b1);
        std::cout << "\nRight-Hand Side Vector b: [ ";
        for (double val : b1) std::cout << val << " ";
        std::cout << "]\n";

        std::cout << "Solution Vector x:        [ ";
        for (double val : x1) std::cout << std::fixed << std::setprecision(4) << val << " ";
        std::cout << "]\n";

        // Verification A * x
        std::cout << "Verification A * x:       [ ";
        for (size_t i = 0; i < A1.size(); ++i) {
            double ax = 0.0;
            for (size_t j = 0; j < A1[i].size(); ++j) {
                ax += A1[i][j] * x1[j];
            }
            std::cout << std::fixed << std::setprecision(4) << ax << " ";
        }
        std::cout << "]\n\n";
    }

    // Example 2: 4x4 Pascal Matrix (always symmetric positive-definite)
    std::vector<std::vector<double>> A2 = {
        { 1.0,  1.0,  1.0,  1.0},
        { 1.0,  2.0,  3.0,  4.0},
        { 1.0,  3.0,  6.0, 10.0},
        { 1.0,  4.0, 10.0, 20.0}
    };
    std::vector<double> b2 = {10.0, 20.0, 35.0, 56.0};

    std::cout << "System 2 Matrix A (4x4 Pascal SPD):\n";
    printMatrix(A2);

    auto res2 = choleskyDecompose(A2);
    if (res2.isPositiveDefinite) {
        std::cout << "\nCholesky Factor L:\n";
        printMatrix(res2.L);

        auto x2 = choleskySolve(res2.L, b2);
        std::cout << "\nSolution Vector x: [ ";
        for (double val : x2) std::cout << std::fixed << std::setprecision(4) << val << " ";
        std::cout << "]\n\n";
    }

    // Example 3: Non-positive definite matrix
    std::vector<std::vector<double>> A3 = {
        { 1.0,  2.0},
        { 2.0,  1.0} // det = 1 - 4 = -3 < 0, indefinite
    };
    std::cout << "System 3 (Indefinite Matrix):\n";
    printMatrix(A3);
    auto res3 = choleskyDecompose(A3);
    std::cout << "Cholesky Decomposition Status: " 
              << (res3.isPositiveDefinite ? "SPD" : "FAILED (Not Positive-Definite as expected)") << '\n';

    return 0;
}
