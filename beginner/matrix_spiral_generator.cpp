/**
 * @file matrix_spiral_generator.cpp
 * @brief Generates an N x N matrix filled with values 1 to N^2 in inward spiral order.
 *
 * Algorithm:
 * - Maintain four boundaries: top, bottom, left, right.
 * - Traverse from left to right along the top boundary, then increment top.
 * - Traverse from top to bottom along the right boundary, then decrement right.
 * - Traverse from right to left along the bottom boundary, then decrement bottom.
 * - Traverse from bottom to top along the left boundary, then increment left.
 * - Repeat until top > bottom or left > right.
 *
 * Time Complexity: O(N^2)
 * Space Complexity: O(1) auxiliary (excluding the N x N output matrix)
 */

#include <iostream>
#include <vector>
#include <iomanip>

/**
 * @brief Generates an N x N matrix populated with 1..N^2 in clockwise spiral order.
 * @param n Dimension of the square matrix.
 * @return 2D vector representing the spiral matrix.
 */
std::vector<std::vector<int>> generateSpiralMatrix(int n) {
    if (n <= 0) return {};

    std::vector<std::vector<int>> matrix(n, std::vector<int>(n, 0));
    int top = 0;
    int bottom = n - 1;
    int left = 0;
    int right = n - 1;
    int val = 1;

    while (top <= bottom && left <= right) {
        // Traverse top row
        for (int c = left; c <= right; ++c) {
            matrix[top][c] = val++;
        }
        top++;

        // Traverse right column
        for (int r = top; r <= bottom; ++r) {
            matrix[r][right] = val++;
        }
        right--;

        // Traverse bottom row
        if (top <= bottom) {
            for (int c = right; c >= left; --c) {
                matrix[bottom][c] = val++;
            }
            bottom--;
        }

        // Traverse left column
        if (left <= right) {
            for (int r = bottom; r >= top; --r) {
                matrix[r][left] = val++;
            }
            left++;
        }
    }

    return matrix;
}

/**
 * @brief Pretty prints a square matrix with aligned columns.
 * @param matrix Matrix to display.
 */
void printMatrix(const std::vector<std::vector<int>>& matrix) {
    if (matrix.empty()) {
        std::cout << "  (Empty matrix)\n";
        return;
    }
    int n = static_cast<int>(matrix.size());
    int maxVal = n * n;
    int width = 0;
    while (maxVal > 0) {
        width++;
        maxVal /= 10;
    }
    width = std::max(width + 1, 3);

    for (const auto& row : matrix) {
        std::cout << "  ";
        for (int val : row) {
            std::cout << std::setw(width) << val;
        }
        std::cout << '\n';
    }
}

int main() {
    std::cout << "=== Spiral Matrix Generator ===\n\n";

    std::vector<int> testSizes = {1, 2, 3, 4, 5};

    for (int n : testSizes) {
        std::cout << "Matrix Size: " << n << " x " << n << '\n';
        auto spiral = generateSpiralMatrix(n);
        printMatrix(spiral);
        std::cout << '\n';
    }

    return 0;
}
