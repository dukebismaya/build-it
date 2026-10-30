/**
 * @file matrix_symmetric_checker.cpp
 * @brief Checks if a square matrix is Symmetric, Skew-Symmetric, or Asymmetric.
 *
 * Definitions:
 * - Symmetric Matrix: A square matrix A where A[i][j] = A[j][i] for all i, j.
 *   (i.e., A equals its transpose A^T).
 * - Skew-Symmetric (Antisymmetric) Matrix: A square matrix A where A[i][j] = -A[j][i] for all i, j.
 *   This implies all main diagonal entries must be zero: A[i][i] = -A[i][i] => A[i][i] = 0.
 *
 * Time Complexity: O(N^2)
 * Space Complexity: O(1) auxiliary space
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

enum class MatrixSymmetryType {
    SYMMETRIC,
    SKEW_SYMMETRIC,
    BOTH_ZERO_MATRIX,
    ASYMMETRIC,
    NOT_SQUARE
};

/**
 * @brief Checks the symmetry classification of a 2D integer matrix.
 * @param mat 2D matrix.
 * @return MatrixSymmetryType enum value.
 */
MatrixSymmetryType classifyMatrixSymmetry(const std::vector<std::vector<int>>& mat) {
    if (mat.empty()) return MatrixSymmetryType::NOT_SQUARE;

    int n = static_cast<int>(mat.size());
    for (int i = 0; i < n; ++i) {
        if (static_cast<int>(mat[i].size()) != n) {
            return MatrixSymmetryType::NOT_SQUARE;
        }
    }

    bool isSymmetric = true;
    bool isSkewSymmetric = true;

    // Check diagonal for skew-symmetry
    for (int i = 0; i < n; ++i) {
        if (mat[i][i] != 0) {
            isSkewSymmetric = false;
            break;
        }
    }

    // Check off-diagonal elements
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (mat[i][j] != mat[j][i]) {
                isSymmetric = false;
            }
            if (mat[i][j] != -mat[j][i]) {
                isSkewSymmetric = false;
            }
            if (!isSymmetric && !isSkewSymmetric) {
                return MatrixSymmetryType::ASYMMETRIC;
            }
        }
    }

    if (isSymmetric && isSkewSymmetric) return MatrixSymmetryType::BOTH_ZERO_MATRIX;
    if (isSymmetric) return MatrixSymmetryType::SYMMETRIC;
    if (isSkewSymmetric) return MatrixSymmetryType::SKEW_SYMMETRIC;
    return MatrixSymmetryType::ASYMMETRIC;
}

void printMatrix(const std::vector<std::vector<int>>& mat) {
    for (const auto& row : mat) {
        std::cout << "  ";
        for (int val : row) {
            std::cout << std::setw(5) << val;
        }
        std::cout << '\n';
    }
}

void evaluateAndDisplay(const std::string& label, const std::vector<std::vector<int>>& mat) {
    std::cout << label << ":\n";
    printMatrix(mat);

    auto result = classifyMatrixSymmetry(mat);
    std::cout << "Classification: ";
    switch (result) {
        case MatrixSymmetryType::SYMMETRIC:
            std::cout << "SYMMETRIC (A == A^T)\n";
            break;
        case MatrixSymmetryType::SKEW_SYMMETRIC:
            std::cout << "SKEW-SYMMETRIC (A == -A^T and diag == 0)\n";
            break;
        case MatrixSymmetryType::BOTH_ZERO_MATRIX:
            std::cout << "BOTH SYMMETRIC & SKEW-SYMMETRIC (Zero Matrix)\n";
            break;
        case MatrixSymmetryType::ASYMMETRIC:
            std::cout << "ASYMMETRIC (Neither)\n";
            break;
        case MatrixSymmetryType::NOT_SQUARE:
            std::cout << "INVALID (Matrix is not square)\n";
            break;
    }
    std::cout << '\n';
}

int main() {
    std::cout << "=== Matrix Symmetry & Skew-Symmetry Checker ===\n\n";

    // Matrix 1: Symmetric
    std::vector<std::vector<int>> m1 = {
        {1, 7, 3},
        {7, 4, -5},
        {3, -5, 6}
    };
    evaluateAndDisplay("1. Symmetric Matrix (3x3)", m1);

    // Matrix 2: Skew-Symmetric
    std::vector<std::vector<int>> m2 = {
        { 0,  2, -5},
        {-2,  0,  4},
        { 5, -4,  0}
    };
    evaluateAndDisplay("2. Skew-Symmetric Matrix (3x3)", m2);

    // Matrix 3: Asymmetric
    std::vector<std::vector<int>> m3 = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    evaluateAndDisplay("3. Asymmetric Matrix (3x3)", m3);

    // Matrix 4: Zero matrix (Both)
    std::vector<std::vector<int>> m4 = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };
    evaluateAndDisplay("4. Zero Matrix (3x3)", m4);

    return 0;
}
