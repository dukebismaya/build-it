/*
 * Program Description:
 * Snake (Wave / Zigzag) Pattern Matrix Traversal in C++.
 *
 * Traversal Patterns:
 * 1. Horizontal Snake Traversal:
 *    - Even rows (0, 2, 4...) are traversed left-to-right.
 *    - Odd rows (1, 3, 5...) are traversed right-to-left.
 * 2. Vertical Snake Traversal:
 *    - Even columns are traversed top-to-bottom.
 *    - Odd columns are traversed bottom-to-top.
 *
 * Time Complexity: O(R * C) where R is row count, C is column count.
 * Space Complexity: O(1) auxiliary space (excluding result).
 */

#include <iostream>
#include <vector>
#include <iomanip>

// Traverses matrix in horizontal snake pattern
std::vector<int> horizontalSnakeTraversal(const std::vector<std::vector<int>>& mat) {
    std::vector<int> result;
    if (mat.empty() || mat[0].empty()) return result;

    int rows = mat.size();
    int cols = mat[0].size();
    result.reserve(rows * cols);

    for (int i = 0; i < rows; ++i) {
        if (i % 2 == 0) {
            // Left to right
            for (int j = 0; j < cols; ++j) {
                result.push_back(mat[i][j]);
            }
        } else {
            // Right to left
            for (int j = cols - 1; j >= 0; --j) {
                result.push_back(mat[i][j]);
            }
        }
    }

    return result;
}

// Traverses matrix in vertical snake pattern
std::vector<int> verticalSnakeTraversal(const std::vector<std::vector<int>>& mat) {
    std::vector<int> result;
    if (mat.empty() || mat[0].empty()) return result;

    int rows = mat.size();
    int cols = mat[0].size();
    result.reserve(rows * cols);

    for (int j = 0; j < cols; ++j) {
        if (j % 2 == 0) {
            // Top to bottom
            for (int i = 0; i < rows; ++i) {
                result.push_back(mat[i][j]);
            }
        } else {
            // Bottom to top
            for (int i = rows - 1; i >= 0; --i) {
                result.push_back(mat[i][j]);
            }
        }
    }

    return result;
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
    std::cout << "        Matrix Snake Traversal Demo       \n";
    std::cout << "==========================================\n\n";

    std::vector<std::vector<int>> mat = {
        {10, 20, 30, 40},
        {15, 25, 35, 45},
        {27, 29, 37, 48},
        {32, 33, 39, 50}
    };

    std::cout << "Input Matrix (4x4):\n";
    printMatrix(mat);

    std::cout << "\nHorizontal Snake Pattern Traversal:\n";
    auto horiz = horizontalSnakeTraversal(mat);
    printVector(horiz);

    std::cout << "\nVertical Snake Pattern Traversal:\n";
    auto vert = verticalSnakeTraversal(mat);
    printVector(vert);

    return 0;
}
