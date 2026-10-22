/*
 * Program Description:
 * Toeplitz Matrix Checker and Generator in C++.
 * A matrix is Toeplitz (or diagonal-constant matrix) if every descending diagonal
 * from top-left to bottom-right has identical elements.
 *
 * Mathematical condition:
 * A matrix M is Toeplitz if and only if:
 * M[i][j] == M[i + 1][j + 1] for all valid 0 <= i < R - 1 and 0 <= j < C - 1.
 *
 * Time Complexity: O(R * C)
 * Space Complexity: O(1) auxiliary memory.
 */

#include <iostream>
#include <vector>
#include <iomanip>

// Checks if matrix is Toeplitz in a single pass in O(R * C)
bool isToeplitzMatrix(const std::vector<std::vector<int>>& mat) {
    if (mat.empty() || mat[0].empty()) return true;

    int rows = mat.size();
    int cols = mat[0].size();

    for (int i = 0; i < rows - 1; ++i) {
        for (int j = 0; j < cols - 1; ++j) {
            if (mat[i][j] != mat[i + 1][j + 1]) {
                return false;
            }
        }
    }

    return true;
}

// Generates an R x C Toeplitz matrix from specified first row and first column
std::vector<std::vector<int>> createToeplitz(int r, int c, const std::vector<int>& row0, const std::vector<int>& col0) {
    std::vector<std::vector<int>> mat(r, std::vector<int>(c, 0));
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            if (j >= i) {
                mat[i][j] = row0[j - i];
            } else {
                mat[i][j] = col0[i - j];
            }
        }
    }
    return mat;
}

void printMatrix(const std::vector<std::vector<int>>& mat) {
    for (const auto& row : mat) {
        std::cout << "  ";
        for (int val : row) {
            std::cout << std::setw(4) << val;
        }
        std::cout << "\n";
    }
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "        Toeplitz Matrix Validator         \n";
    std::cout << "==========================================\n\n";

    // Test Case 1: Valid Toeplitz matrix
    // 1  2  3  4
    // 5  1  2  3
    // 9  5  1  2
    std::vector<std::vector<int>> mat1 = {
        {1, 2, 3, 4},
        {5, 1, 2, 3},
        {9, 5, 1, 2}
    };

    std::cout << "Matrix 1 (3x4):\n";
    printMatrix(mat1);
    bool check1 = isToeplitzMatrix(mat1);
    std::cout << "Is Toeplitz? " << (check1 ? "YES [PASS]" : "NO [FAIL]") << "\n\n";

    // Test Case 2: Non-Toeplitz matrix
    std::vector<std::vector<int>> mat2 = {
        {1, 2},
        {2, 2}
    };
    std::cout << "Matrix 2 (2x2):\n";
    printMatrix(mat2);
    bool check2 = isToeplitzMatrix(mat2);
    std::cout << "Is Toeplitz? " << (check2 ? "YES" : "NO [CORRECT: diagonal 1!=2]") << "\n\n";

    // Test Case 3: Generating a 4x5 Toeplitz matrix
    std::vector<int> row0 = {7, 8, 9, 10, 11};
    std::vector<int> col0 = {7, 6, 5, 4};
    auto mat3 = createToeplitz(4, 5, row0, col0);

    std::cout << "Generated 4x5 Toeplitz Matrix:\n";
    printMatrix(mat3);
    bool check3 = isToeplitzMatrix(mat3);
    std::cout << "Verification Check: " << (check3 ? "CONFIRMED VALID [PASS]" : "FAILED") << "\n";

    return 0;
}
