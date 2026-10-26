/*
 * Program Description:
 * Matrix Anti-Diagonal Traversal in C++.
 * Given an M x N matrix, traverses and groups elements along all anti-diagonals.
 * An anti-diagonal consists of all cells (i, j) where the sum of indices (i + j) is constant.
 *
 * For an M x N matrix:
 * - There are exactly (M + N - 1) anti-diagonals, with sum d = i + j from 0 to M + N - 2.
 * - Each anti-diagonal starts from the top row (or rightmost column) and proceeds downwards-left.
 *
 * Time Complexity: O(M * N)
 * Space Complexity: O(M * N) to store the grouped diagonals.
 */

#include <iostream>
#include <vector>
#include <iomanip>

std::vector<std::vector<int>> getAntiDiagonals(const std::vector<std::vector<int>>& mat) {
    if (mat.empty() || mat[0].empty()) return {};

    int rows = mat.size();
    int cols = mat[0].size();
    int numDiagonals = rows + cols - 1;

    std::vector<std::vector<int>> diagonals(numDiagonals);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            diagonals[i + j].push_back(mat[i][j]);
        }
    }

    return diagonals;
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
    std::cout << "     Matrix Anti-Diagonal Traversal       \n";
    std::cout << "==========================================\n\n";

    // Test Case: 3x4 Matrix
    std::vector<std::vector<int>> mat = {
        { 1,  2,  3,  4},
        { 5,  6,  7,  8},
        { 9, 10, 11, 12}
    };

    std::cout << "Input Matrix (3x4):\n";
    printMatrix(mat);

    auto diagonals = getAntiDiagonals(mat);

    std::cout << "\nAnti-Diagonal Groups (i + j = constant):\n";
    for (size_t d = 0; d < diagonals.size(); ++d) {
        std::cout << "  Diagonal #" << d << " (sum = " << d << "): [";
        for (size_t k = 0; k < diagonals[d].size(); ++k) {
            std::cout << diagonals[d][k] << (k + 1 < diagonals[d].size() ? ", " : "");
        }
        std::cout << "]\n";
    }

    return 0;
}
