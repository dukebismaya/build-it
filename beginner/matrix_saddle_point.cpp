/*
 * Program Description:
 * Matrix Saddle Point Finder in C++.
 * A saddle point of a matrix is an element that is simultaneously:
 * 1. The minimum element in its row, AND
 * 2. The maximum element in its column.
 *
 * Concepts covered:
 * - 2D matrix traversal and coordinate manipulation
 * - Min-row and max-column determination
 * - Verification and edge cases (no saddle point, single saddle point, ties)
 *
 * Time Complexity: O(R * C) where R is row count and C is column count.
 * Space Complexity: O(R + C) for row-minima and col-maxima precomputation.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <climits>

struct SaddlePoint {
    int row;
    int col;
    int value;
};

// Finds all saddle points in the given matrix
std::vector<SaddlePoint> findSaddlePoints(const std::vector<std::vector<int>>& mat) {
    std::vector<SaddlePoint> points;
    if (mat.empty() || mat[0].empty()) return points;

    int rows = mat.size();
    int cols = mat[0].size();

    // Find row minimums
    std::vector<int> rowMin(rows, INT_MAX);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (mat[i][j] < rowMin[i]) {
                rowMin[i] = mat[i][j];
            }
        }
    }

    // Find column maximums
    std::vector<int> colMax(cols, INT_MIN);
    for (int j = 0; j < cols; ++j) {
        for (int i = 0; i < rows; ++i) {
            if (mat[i][j] > colMax[j]) {
                colMax[j] = mat[i][j];
            }
        }
    }

    // Identify elements that match both criteria
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (mat[i][j] == rowMin[i] && mat[i][j] == colMax[j]) {
                points.push_back({i, j, mat[i][j]});
            }
        }
    }

    return points;
}

void printMatrix(const std::vector<std::vector<int>>& mat) {
    for (const auto& row : mat) {
        std::cout << "  ";
        for (int val : row) {
            std::cout << std::setw(5) << val;
        }
        std::cout << "\n";
    }
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "        Matrix Saddle Point Finder        \n";
    std::cout << "==========================================\n\n";

    // Test Case 1: Saddle point at (0, 0) with value 7
    std::vector<std::vector<int>> mat1 = {
        {7, 8, 9},
        {4, 5, 6},
        {1, 2, 3}
    };

    std::cout << "Matrix 1:\n";
    printMatrix(mat1);
    auto points1 = findSaddlePoints(mat1);
    if (points1.empty()) {
        std::cout << "Result: No saddle point found.\n\n";
    } else {
        for (const auto& sp : points1) {
            std::cout << "Result: Saddle point found at (" << sp.row << ", " << sp.col
                      << ") with value = " << sp.value << "\n\n";
        }
    }

    // Test Case 2: Saddle point at (1, 1) with value 5
    std::vector<std::vector<int>> mat2 = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    std::cout << "Matrix 2:\n";
    printMatrix(mat2);
    auto points2 = findSaddlePoints(mat2);
    if (points2.empty()) {
        std::cout << "Result: No saddle point found (Min-row values don't match Max-col).\n\n";
    } else {
        for (const auto& sp : points2) {
            std::cout << "Result: Saddle point at (" << sp.row << ", " << sp.col << ") = " << sp.value << "\n\n";
        }
    }

    // Test Case 3: 4x4 matrix
    std::vector<std::vector<int>> mat3 = {
        {1, 10, 4},
        {9, 3,  8},
        {15, 16, 17}
    };
    std::cout << "Matrix 3:\n";
    printMatrix(mat3);
    auto points3 = findSaddlePoints(mat3);
    if (points3.empty()) {
        std::cout << "Result: No saddle point found.\n";
    } else {
        for (const auto& sp : points3) {
            std::cout << "Result: Saddle point at (" << sp.row << ", " << sp.col << ") = " << sp.value << "\n";
        }
    }

    return 0;
}
