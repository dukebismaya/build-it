#include <iostream>
#include <vector>
#include <iomanip>

// Function to print a matrix
void printMatrix(const std::vector<std::vector<int>>& mat, const std::string& name) {
    std::cout << name << ":\n";
    for (const auto& row : mat) {
        for (int val : row) {
            std::cout << std::setw(4) << val;
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

// Function to rotate a matrix 90 degrees clockwise (in-place for square matrices)
std::vector<std::vector<int>> rotateClockwise90(const std::vector<std::vector<int>>& mat) {
    int rows = mat.size();
    int cols = mat[0].size();
    std::vector<std::vector<int>> rotated(cols, std::vector<int>(rows));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            rotated[j][rows - 1 - i] = mat[i][j];
        }
    }
    return rotated;
}

int main() {
    std::vector<std::vector<int>> matrix = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9,  10, 11, 12},
        {13, 14, 15, 16}
    };

    printMatrix(matrix, "Original Matrix");

    auto rot90 = rotateClockwise90(matrix);
    printMatrix(rot90, "Rotated 90° Clockwise");

    auto rot180 = rotateClockwise90(rot90);
    printMatrix(rot180, "Rotated 180°");

    auto rot270 = rotateClockwise90(rot180);
    printMatrix(rot270, "Rotated 270° Clockwise (90° Counter-Clockwise)");

    return 0;
}
