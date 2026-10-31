/**
 * @file matrix_trace_calculator.cpp
 * @brief Computes the Trace and Anti-Trace of square matrices and verifies algebraic trace properties.
 *
 * Theory:
 * - The Trace of an n x n square matrix A, denoted Tr(A), is the sum of its main diagonal elements:
 *   Tr(A) = sum_{i=0}^{n-1} A[i][i]
 * - The Anti-Trace is the sum of its anti-diagonal (secondary diagonal) elements:
 *   AntiTr(A) = sum_{i=0}^{n-1} A[i][n - 1 - i]
 *
 * Linear Algebraic Properties Verified:
 * 1. Linearity: Tr(A + B) = Tr(A) + Tr(B)
 * 2. Scalar Multiplication: Tr(c * A) = c * Tr(A)
 * 3. Transposition Invariance: Tr(A) = Tr(A^T)
 *
 * Time Complexity: O(N) to compute trace, O(N^2) to verify matrix operations
 * Space Complexity: O(1) auxiliary space (O(N^2) for matrix addition/transpose)
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

/**
 * @brief Computes the main diagonal trace of an n x n square matrix.
 * @param mat Square matrix.
 * @return Sum of main diagonal elements.
 */
long long computeTrace(const std::vector<std::vector<int>>& mat) {
    int n = static_cast<int>(mat.size());
    long long trace = 0;
    for (int i = 0; i < n; ++i) {
        trace += mat[i][i];
    }
    return trace;
}

/**
 * @brief Computes the anti-diagonal (secondary diagonal) trace of an n x n square matrix.
 * @param mat Square matrix.
 * @return Sum of anti-diagonal elements.
 */
long long computeAntiTrace(const std::vector<std::vector<int>>& mat) {
    int n = static_cast<int>(mat.size());
    long long antiTrace = 0;
    for (int i = 0; i < n; ++i) {
        antiTrace += mat[i][n - 1 - i];
    }
    return antiTrace;
}

/**
 * @brief Computes the transpose of an n x n matrix.
 */
std::vector<std::vector<int>> transposeMatrix(const std::vector<std::vector<int>>& mat) {
    int n = static_cast<int>(mat.size());
    std::vector<std::vector<int>> res(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            res[j][i] = mat[i][j];
        }
    }
    return res;
}

/**
 * @brief Adds two n x n matrices element-wise.
 */
std::vector<std::vector<int>> addMatrices(const std::vector<std::vector<int>>& A,
                                         const std::vector<std::vector<int>>& B) {
    int n = static_cast<int>(A.size());
    std::vector<std::vector<int>> C(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
    return C;
}

/**
 * @brief Multiplies an n x n matrix by a scalar.
 */
std::vector<std::vector<int>> scalarMultiply(const std::vector<std::vector<int>>& A, int c) {
    int n = static_cast<int>(A.size());
    std::vector<std::vector<int>> res(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            res[i][j] = A[i][j] * c;
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
    std::cout << "=== Matrix Trace and Anti-Trace Calculator ===\n\n";

    // 1. Compute Trace & Anti-Trace on a 4x4 matrix
    std::vector<std::vector<int>> A = {
        { 5,  8, -3,  2},
        { 4,  1,  9, -6},
        { 0,  7, 11,  3},
        {-2,  6,  4,  8}
    };

    std::cout << "Matrix A (4x4):\n";
    printMatrix(A);

    long long trA = computeTrace(A);
    long long antiTrA = computeAntiTrace(A);

    std::cout << "\nMain Diagonal Elements:  "
              << A[0][0] << " + " << A[1][1] << " + " << A[2][2] << " + " << A[3][3] << '\n';
    std::cout << "Trace(A):                " << trA << '\n';

    std::cout << "\nAnti-Diagonal Elements:  "
              << A[0][3] << " + " << A[1][2] << " + " << A[2][1] << " + " << A[3][0] << '\n';
    std::cout << "Anti-Trace(A):           " << antiTrA << "\n\n";

    // 2. Verification of Trace Properties
    std::cout << "--- Verifying Linear Algebraic Properties ---\n";

    std::vector<std::vector<int>> B = {
        { 2, -1,  4,  7},
        { 3,  5,  0,  1},
        {-4,  2, -3,  8},
        { 6,  9,  1,  4}
    };
    long long trB = computeTrace(B);

    // Property 1: Tr(A + B) = Tr(A) + Tr(B)
    auto sumAB = addMatrices(A, B);
    long long trSumAB = computeTrace(sumAB);
    std::cout << "1. Additivity: Tr(A + B) == Tr(A) + Tr(B)\n"
              << "   Tr(A + B)       = " << trSumAB << '\n'
              << "   Tr(A) + Tr(B)   = " << trA << " + " << trB << " = " << (trA + trB) << '\n'
              << "   Property holds? : " << (trSumAB == trA + trB ? "PASS" : "FAIL") << "\n\n";

    // Property 2: Tr(c * A) = c * Tr(A)
    int scalar = 7;
    auto scaledA = scalarMultiply(A, scalar);
    long long trScaledA = computeTrace(scaledA);
    std::cout << "2. Homogeneity: Tr(c * A) == c * Tr(A) (with c = " << scalar << ")\n"
              << "   Tr(" << scalar << " * A)       = " << trScaledA << '\n'
              << "   " << scalar << " * Tr(A)       = " << scalar << " * " << trA << " = " << (scalar * trA) << '\n'
              << "   Property holds? : " << (trScaledA == scalar * trA ? "PASS" : "FAIL") << "\n\n";

    // Property 3: Tr(A) = Tr(A^T)
    auto transpA = transposeMatrix(A);
    long long trTranspA = computeTrace(transpA);
    std::cout << "3. Transposition: Tr(A) == Tr(A^T)\n"
              << "   Tr(A)           = " << trA << '\n'
              << "   Tr(A^T)         = " << trTranspA << '\n'
              << "   Property holds? : " << (trA == trTranspA ? "PASS" : "FAIL") << '\n';

    return 0;
}
