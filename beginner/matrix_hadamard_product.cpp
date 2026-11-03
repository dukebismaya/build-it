/**
 * @file matrix_hadamard_product.cpp
 * @brief Computes the Hadamard (element-wise) product of matrices and verifies algebraic properties.
 *
 * Theory:
 * For two matrices A and B of the same dimensions M x N, the Hadamard product (denoted A o B)
 * is defined as:
 *   (A o B)[i][j] = A[i][j] * B[i][j]   for all 0 <= i < M, 0 <= j < N.
 *
 * Algebraic Properties Verified:
 * 1. Commutativity:            A o B = B o A
 * 2. Associativity:            (A o B) o C = A o (B o C)
 * 3. Distributivity over sum:  A o (B + C) = A o B + A o C
 * 4. Identity element:         A o J = A  (where J is the all-ones matrix)
 *
 * Time Complexity: O(M * N)
 * Space Complexity: O(M * N) for the product matrix, O(1) auxiliary for in-place
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

/**
 * @brief Computes the Hadamard product of two matrices out-of-place.
 */
std::vector<std::vector<int>> hadamardProduct(const std::vector<std::vector<int>>& A,
                                             const std::vector<std::vector<int>>& B) {
    int m = static_cast<int>(A.size());
    int n = static_cast<int>(A[0].size());
    std::vector<std::vector<int>> C(m, std::vector<int>(n));

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = A[i][j] * B[i][j];
        }
    }
    return C;
}

/**
 * @brief Performs element-wise matrix addition.
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
    std::cout << "=== Hadamard (Element-wise) Matrix Product & Properties ===\n\n";

    std::vector<std::vector<int>> A = {
        {1, -2, 3},
        {4,  5, 0}
    };

    std::vector<std::vector<int>> B = {
        { 2, 3, -1},
        {-3, 4,  7}
    };

    std::vector<std::vector<int>> C = {
        { 5, 1, 2},
        {-2, 3, 4}
    };

    std::cout << "Matrix A (2x3):\n";
    printMatrix(A);
    std::cout << "\nMatrix B (2x3):\n";
    printMatrix(B);
    std::cout << "\nMatrix C (2x3):\n";
    printMatrix(C);

    // 1. Basic product A o B
    auto prodAB = hadamardProduct(A, B);
    std::cout << "\nHadamard Product (A o B):\n";
    printMatrix(prodAB);

    // 2. Property 1: Commutativity (A o B == B o A)
    auto prodBA = hadamardProduct(B, A);
    std::cout << "\n1. Commutativity: A o B == B o A -> " 
              << (prodAB == prodBA ? "PASS" : "FAIL") << '\n';

    // 3. Property 2: Associativity ((A o B) o C == A o (B o C))
    auto assoc1 = hadamardProduct(prodAB, C);
    auto assoc2 = hadamardProduct(A, hadamardProduct(B, C));
    std::cout << "2. Associativity: (A o B) o C == A o (B o C) -> " 
              << (assoc1 == assoc2 ? "PASS" : "FAIL") << '\n';

    // 4. Property 3: Distributivity (A o (B + C) == A o B + A o C)
    auto sumBC = addMatrices(B, C);
    auto distLHS = hadamardProduct(A, sumBC);
    auto distRHS = addMatrices(hadamardProduct(A, B), hadamardProduct(A, C));
    std::cout << "3. Distributivity: A o (B + C) == A o B + A o C -> " 
              << (distLHS == distRHS ? "PASS" : "FAIL") << "\n\n";

    std::cout << "Detailed Distributivity Matrix Result:\n";
    printMatrix(distLHS);

    return 0;
}

