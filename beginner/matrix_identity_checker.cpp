/*
 * Program Description:
 * Identity Matrix Checker in C++.
 * An Identity Matrix is a square matrix of size N x N where:
 * 1. All elements on the principal diagonal are equal to 1: M[i][i] == 1.
 * 2. All off-diagonal elements are equal to 0: M[i][j] == 0 for all i != j.
 *
 * Concepts:
 * - Square matrix dimension validation
 * - Efficient early exit traversal
 *
 * Time Complexity: O(N^2) worst case, O(1) best case.
 * Space Complexity: O(1).
 */

#include <iostream>
#include <vector>
#include <iomanip>

bool isIdentityMatrix(const std::vector<std::vector<int>>& mat) {
    if (mat.empty() || mat[0].empty()) return false;

    int rows = mat.size();
    int cols = mat[0].size();

    // Must be a square matrix
    if (rows != cols) return false;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (i == j) {
                if (mat[i][j] != 1) return false;
            } else {
                if (mat[i][j] != 0) return false;
            }
        }
    }

    return true;
}

void printMatrix(const std::vector<std::vector<int>>& mat) {
    for (const auto& row : mat) {
        std::cout << "  ";
        for (int val : row) {
            std::cout << std::setw(3) << val;
        }
        std::cout << "\n";
    }
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "         Identity Matrix Checker          \n";
    std::cout << "==========================================\n\n";

    // Test Case 1: Valid 3x3 identity matrix
    std::vector<std::vector<int>> mat1 = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };
    std::cout << "Matrix 1 (3x3):\n";
    printMatrix(mat1);
    std::cout << "Is Identity? " << (isIdentityMatrix(mat1) ? "YES [PASS]" : "NO [FAIL]") << "\n\n";

    // Test Case 2: Non-identity square matrix
    std::vector<std::vector<int>> mat2 = {
        {1, 0, 2},
        {0, 1, 0},
        {0, 0, 1}
    };
    std::cout << "Matrix 2 (3x3 with off-diagonal 2):\n";
    printMatrix(mat2);
    std::cout << "Is Identity? " << (isIdentityMatrix(mat2) ? "YES" : "NO [PASS: detected 2]") << "\n\n";

    // Test Case 3: Valid 4x4 identity matrix
    std::vector<std::vector<int>> mat3 = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
    std::cout << "Matrix 3 (4x4):\n";
    printMatrix(mat3);
    std::cout << "Is Identity? " << (isIdentityMatrix(mat3) ? "YES [PASS]" : "NO") << "\n";

    return 0;
}
