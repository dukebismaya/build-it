/*
 * Program Description:
 * Matrix Diagonal Sum and Symmetry Analyzer in C++.
 * Given an N x N square matrix:
 * 1. Primary (Main) Diagonal: elements from top-left to bottom-right (M[i][i]).
 * 2. Secondary (Anti) Diagonal: elements from top-right to bottom-left (M[i][N - 1 - i]).
 * 3. Total Diagonal Sum: sum of all elements that lie on either diagonal.
 *    If N is odd, the exact center element M[N/2][N/2] is shared between both diagonals
 *    and must be counted only once.
 * 4. Checks whether the matrix is symmetric across its main diagonal (M[i][j] == M[j][i]).
 *
 * Time Complexity: O(N) for diagonal sums, O(N^2) for symmetry test.
 * Space Complexity: O(1).
 */

#include <iostream>
#include <vector>
#include <iomanip>

struct DiagonalAnalysis {
    long long primarySum = 0;
    long long secondarySum = 0;
    long long totalUniqueSum = 0;
    bool isSymmetric = true;
};

DiagonalAnalysis analyzeDiagonals(const std::vector<std::vector<int>>& mat) {
    DiagonalAnalysis result;
    int n = mat.size();
    if (n == 0) return result;

    // Single pass O(N) for diagonal sums
    for (int i = 0; i < n; ++i) {
        result.primarySum += mat[i][i];
        result.secondarySum += mat[i][n - 1 - i];

        result.totalUniqueSum += mat[i][i];
        if (i != n - 1 - i) {
            result.totalUniqueSum += mat[i][n - 1 - i];
        }
    }

    // Check symmetry: M[i][j] == M[j][i]
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (mat[i][j] != mat[j][i]) {
                result.isSymmetric = false;
                break;
            }
        }
        if (!result.isSymmetric) break;
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

int main() {
    std::cout << "==========================================\n";
    std::cout << "        Matrix Diagonal Sum Analyzer      \n";
    std::cout << "==========================================\n\n";

    // Test Case 1: 3x3 matrix (odd N, center element = 5)
    std::vector<std::vector<int>> mat1 = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    std::cout << "Matrix 1 (3x3, odd N):\n";
    printMatrix(mat1);

    auto res1 = analyzeDiagonals(mat1);
    std::cout << "  Primary Diagonal Sum   : " << res1.primarySum << " (1 + 5 + 9 = 15)\n";
    std::cout << "  Secondary Diagonal Sum : " << res1.secondarySum << " (3 + 5 + 7 = 15)\n";
    std::cout << "  Total Unique Diag Sum  : " << res1.totalUniqueSum
              << " (1+5+9 + 3+7 = 25, center 5 counted once)\n";
    std::cout << "  Is Symmetric?          : " << (res1.isSymmetric ? "YES" : "NO") << "\n\n";

    // Test Case 2: 4x4 symmetric matrix (even N)
    std::vector<std::vector<int>> mat2 = {
        {1, 0, 0, 1},
        {0, 2, 2, 0},
        {0, 2, 2, 0},
        {1, 0, 0, 1}
    };

    std::cout << "Matrix 2 (4x4, symmetric):\n";
    printMatrix(mat2);

    auto res2 = analyzeDiagonals(mat2);
    std::cout << "  Primary Diagonal Sum   : " << res2.primarySum << "\n";
    std::cout << "  Secondary Diagonal Sum : " << res2.secondarySum << "\n";
    std::cout << "  Total Unique Diag Sum  : " << res2.totalUniqueSum << "\n";
    std::cout << "  Is Symmetric?          : " << (res2.isSymmetric ? "YES [CONFIRMED]" : "NO") << "\n";

    return 0;
}
