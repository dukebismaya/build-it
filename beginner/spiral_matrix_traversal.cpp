#include <iostream>
#include <vector>

// Function to print a matrix in spiral order
// Traverses the outer boundary, then shrinks inward.
void spiralOrder(const std::vector<std::vector<int>>& matrix) {
    if (matrix.empty()) return;

    int top = 0, bottom = matrix.size() - 1;
    int left = 0, right = matrix[0].size() - 1;
    bool first = true;

    while (top <= bottom && left <= right) {
        // Traverse right across top row
        for (int j = left; j <= right; ++j) {
            if (!first) std::cout << ", ";
            std::cout << matrix[top][j];
            first = false;
        }
        top++;

        // Traverse down along right column
        for (int i = top; i <= bottom; ++i) {
            std::cout << ", " << matrix[i][right];
        }
        right--;

        // Traverse left across bottom row
        if (top <= bottom) {
            for (int j = right; j >= left; --j) {
                std::cout << ", " << matrix[bottom][j];
            }
            bottom--;
        }

        // Traverse up along left column
        if (left <= right) {
            for (int i = bottom; i >= top; --i) {
                std::cout << ", " << matrix[i][left];
            }
            left++;
        }
    }
    std::cout << std::endl;
}

int main() {
    std::vector<std::vector<int>> matrix1 = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9,  10, 11, 12},
        {13, 14, 15, 16}
    };

    std::vector<std::vector<int>> matrix2 = {
        {1, 2, 3, 4, 5, 6},
        {7, 8, 9, 10, 11, 12},
        {13, 14, 15, 16, 17, 18}
    };

    std::cout << "Spiral Matrix Traversal\n\n";
    std::cout << "4x4 Matrix spiral: ";
    spiralOrder(matrix1);

    std::cout << "3x6 Matrix spiral: ";
    spiralOrder(matrix2);

    return 0;
}
