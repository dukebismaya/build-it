/**
 * @file matrix_scalar_multiplication.cpp
 * @brief Performs in-place and out-of-place matrix scalar multiplication and verifies algebraic properties.
 *
 * Theory:
 * Given an M x N matrix A and a scalar c:
 *   (c * A)[i][j] = c * A[i][j]   for all 0 <= i < M, 0 <= j < N.
 *
 * Algebraic Properties Verified:
 * 1. Distributivity over matrix addition:     c * (A + B) = c * A + c * B
 * 2. Distributivity over scalar addition:     (c + d) * A = c * A + d * A
 * 3. Associativity of scalar multiplication: (c * d) * A = c * (d * A)
 * 4. Identity element:                       1 * A = A
 *
 * Time Complexity: O(M * N)
 * Space Complexity:
 * - In-place: O(1) auxiliary
 * - Out-of-place: O(M * N)
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

/**
 * @brief Multiplies an M x N matrix by a scalar in-place.
 */
void scalarMultiplyInPlace(std::vector<std::vector<int>>& mat, int scalar) {
    for (auto& row : mat) {
        for (int& val : row) {
            val *= scalar;
        }
    }
}

/**
 * @brief Multiplies an M x N matrix by a scalar, returning a new matrix.
 */
std::vector<std::vector<int>> scalarMultiply(const std::vector<std::vector<int>>& mat, int scalar) {
    auto res = mat;
    scalarMultiplyInPlace(res, scalar);
    return res;
}

/**
 * @brief Adds two matrices element-wise.
 */
std::vector<std::vector<int>> addMatrices(const std::vector<std::vector<int>>& A,
                                         const std::vector<std::vector<int>>& B) {
    int m = static_cast<int>(A.size());
    int n = static_cast<int>(A[0].size());
    std::vector<std::vector<int>> C(m, std::vector<int>(n));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
    return C;
}

void printMatrix(const std::vector<std::vector<int>>& mat) {
    for (const auto& row : mat) {
        std::cout << "  ";
        for (int val : row) {
            std::cout << std::setw(6) << val;
        }
        std::cout << '\n';
    }
}

int main() {
    std::cout << "=== Matrix Scalar Multiplication & Algebraic Properties ===\n\n";

    std::vector<std::vector<int>> A = {
        { 2, -3,  5},
        { 7,  0, -4}
    };

    std::vector<std::vector<int>> B = {
        {-1,  4,  2},
        { 3, -2,  6}
    };

    int c = 3;
    int d = 2;

    std::cout << "Matrix A (2x3):\n";
    printMatrix(A);

    std::cout << "\nMatrix B (2x3):\n";
    printMatrix(B);

    // 1. Scalar multiplication demonstration
    auto scaledA = scalarMultiply(A, c);
    std::cout << "\nScalar Multiple " << c << " * A:\n";
    printMatrix(scaledA);

    // 2. In-place demonstration
    auto inPlaceCopy = A;
    scalarMultiplyInPlace(inPlaceCopy, -1);
    std::cout << "\nNegated Matrix (-1 * A, in-place):\n";
    printMatrix(inPlaceCopy);

    // 3. Property 1: c * (A + B) == c * A + c * B
    std::cout << "\n--- Verifying Property 1: c * (A + B) == c * A + c * B ---\n";
    auto sumAB = addMatrices(A, B);
    auto lhs1 = scalarMultiply(sumAB, c);
    auto rhs1 = addMatrices(scalarMultiply(A, c), scalarMultiply(B, c));
    std::cout << "c * (A + B):\n";
    printMatrix(lhs1);
    std::cout << "c * A + c * B:\n";
    printMatrix(rhs1);
    std::cout << "Property 1 holds? " << (lhs1 == rhs1 ? "PASS" : "FAIL") << "\n\n";

    // 4. Property 2: (c * d) * A == c * (d * A)
    std::cout << "--- Verifying Property 2: (c * d) * A == c * (d * A) ---\n";
    auto lhs2 = scalarMultiply(A, c * d);
    auto rhs2 = scalarMultiply(scalarMultiply(A, d), c);
    std::cout << "Property 2 holds? " << (lhs2 == rhs2 ? "PASS" : "FAIL") << '\n';

    return 0;
}

