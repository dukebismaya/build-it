/*
 * Program Description:
 * Matrix Boundary Traversal in C++.
 * Given an M x N 2D matrix, traverses and prints its boundary elements in clockwise order:
 * 1. Top row (from left to right)
 * 2. Rightmost column (from top to bottom)
 * 3. Bottom row (from right to left, if rows > 1)
 * 4. Leftmost column (from bottom to top, if columns > 1)
 *
 * Edge cases handled:
 * - Single row matrix (1 x N)
 * - Single column matrix (M x 1)
 * - Single element matrix (1 x 1)
 *
 * Time Complexity: O(M + N)
 * Space Complexity: O(1) auxiliary space (excluding result).
 */

#include <iostream>
#include <vector>
#include <iomanip>

std::vector<int> boundaryTraversal(const std::vector<std::vector<int>>& mat) {
    std::vector<int> boundary;
    if (mat.empty() || mat[0].empty()) return boundary;

    int rows = mat.size();
    int cols = mat[0].size();

    // Case 1: Single row
    if (rows == 1) {
        for (int j = 0; j < cols; ++j) {
            boundary.push_back(mat[0][j]);
        }
        return boundary;
    }

    // Case 2: Single column
    if (cols == 1) {
        for (int i = 0; i < rows; ++i) {
            boundary.push_back(mat[i][0]);
        }
        return boundary;
    }

    // General case:
    // 1. Top row (left to right)
    for (int j = 0; j < cols; ++j) {
        boundary.push_back(mat[0][j]);
    }

    // 2. Right column (top + 1 to bottom - 1)
    for (int i = 1; i < rows - 1; ++i) {
        boundary.push_back(mat[i][cols - 1]);
    }

    // 3. Bottom row (right to left)
    for (int j = cols - 1; j >= 0; --j) {
        boundary.push_back(mat[rows - 1][j]);
    }

    // 4. Left column (bottom - 1 to top + 1)
    for (int i = rows - 2; i >= 1; --i) {
        boundary.push_back(mat[i][0]);
    }

    return boundary;
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

void printVector(const std::vector<int>& vec) {
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << (i + 1 < vec.size() ? ", " : "");
    }
    std::cout << "]\n";
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "        Matrix Boundary Traversal         \n";
    std::cout << "==========================================\n\n";

    // Test Case 1: 4x4 matrix
    std::vector<std::vector<int>> mat1 = {
        { 1,  2,  3,  4},
        { 5,  6,  7,  8},
        { 9, 10, 11, 12},
        {13, 14, 15, 16}
    };

    std::cout << "Matrix 1 (4x4):\n";
    printMatrix(mat1);
    std::cout << "Clockwise Boundary: ";
    printVector(boundaryTraversal(mat1));

    // Test Case 2: 3x5 matrix
    std::vector<std::vector<int>> mat2 = {
        {1, 2, 3, 4, 5},
        {6, 7, 8, 9, 10},
        {11, 12, 13, 14, 15}
    };
    std::cout << "\nMatrix 2 (3x5):\n";
    printMatrix(mat2);
    std::cout << "Clockwise Boundary: ";
    printVector(boundaryTraversal(mat2));

    // Test Case 3: 1x4 single row
    std::vector<std::vector<int>> mat3 = {{10, 20, 30, 40}};
    std::cout << "\nMatrix 3 (1x4 single row):\n";
    printMatrix(mat3);
    std::cout << "Clockwise Boundary: ";
    printVector(boundaryTraversal(mat3));

    return 0;
}
