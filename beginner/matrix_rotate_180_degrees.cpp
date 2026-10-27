/*
 * Program Description:
 * In-Place 180-Degree Matrix Rotation in C++.
 * Given an R x C 2D matrix, rotates the matrix by 180 degrees clockwise (or counter-clockwise).
 *
 * Algorithm Details:
 * Rotating a matrix by 180 degrees is equivalent to:
 * 1. Reversing the order of the rows (upside-down flip).
 * 2. Reversing the elements within each row (left-to-right flip).
 * Alternatively: swap elements symmetrically across center: M[i][j] <-> M[R - 1 - i][C - 1 - j].
 *
 * Time Complexity: O(R * C)
 * Space Complexity: O(1) in-place.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

void rotateMatrix180InPlace(std::vector<std::vector<int>>& mat) {
    if (mat.empty() || mat[0].empty()) return;

    int rows = mat.size();

    // 1. Reverse rows top-to-bottom
    std::reverse(mat.begin(), mat.end());

    // 2. Reverse each row left-to-right
    for (int i = 0; i < rows; ++i) {
        std::reverse(mat[i].begin(), mat[i].end());
    }
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
    std::cout << "     Matrix 180-Degree In-Place Rotate    \n";
    std::cout << "==========================================\n\n";

    // Test Case 1: 3x3 square matrix
    std::vector<std::vector<int>> mat1 = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    std::cout << "Original Matrix 1 (3x3):\n";
    printMatrix(mat1);

    rotateMatrix180InPlace(mat1);

    std::cout << "\nRotated 180 Degrees:\n";
    printMatrix(mat1);

    // Test Case 2: 2x4 rectangular matrix
    std::vector<std::vector<int>> mat2 = {
        {10, 20, 30, 40},
        {50, 60, 70, 80}
    };

    std::cout << "\nOriginal Matrix 2 (2x4):\n";
    printMatrix(mat2);

    rotateMatrix180InPlace(mat2);

    std::cout << "\nRotated 180 Degrees:\n";
    printMatrix(mat2);

    return 0;
}
