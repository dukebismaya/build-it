/**
 * @file matrix_row_column_sum.cpp
 * @brief Computes individual row sums, column sums, and grand total for an M x N matrix.
 *
 * Concepts:
 * - Given an M x N matrix A:
 *   - Row sum for row i:    RowSum[i] = sum_{j=0}^{N-1} A[i][j]
 *   - Column sum for col j: ColSum[j] = sum_{i=0}^{M-1} A[i][j]
 *   - Grand Total:          Total = sum_{i=0}^{M-1} sum_{j=0}^{N-1} A[i][j]
 *   - Maximum Row Sum:      max_i RowSum[i]
 *   - Maximum Column Sum:   max_j ColSum[j]
 *
 * Time Complexity: O(M * N)
 * Space Complexity: O(M + N) to store row and column totals
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>

struct MatrixSummary {
    std::vector<long long> rowSums;
    std::vector<long long> colSums;
    long long grandTotal;
    int maxRowIndex;
    int maxColIndex;
};

/**
 * @brief Computes row sums, column sums, and extrema for an M x N matrix.
 */
MatrixSummary analyzeMatrixSums(const std::vector<std::vector<int>>& mat) {
    if (mat.empty() || mat[0].empty()) {
        return {{}, {}, 0, -1, -1};
    }

    int m = static_cast<int>(mat.size());
    int n = static_cast<int>(mat[0].size());

    std::vector<long long> rowSums(m, 0);
    std::vector<long long> colSums(n, 0);
    long long total = 0;

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            rowSums[i] += mat[i][j];
            colSums[j] += mat[i][j];
            total += mat[i][j];
        }
    }

    int maxRow = 0;
    for (int i = 1; i < m; ++i) {
        if (rowSums[i] > rowSums[maxRow]) {
            maxRow = i;
        }
    }

    int maxCol = 0;
    for (int j = 1; j < n; ++j) {
        if (colSums[j] > colSums[maxCol]) {
            maxCol = j;
        }
    }

    return {rowSums, colSums, total, maxRow, maxCol};
}

/**
 * @brief Prints the matrix bordered by row sums on the right and column sums at the bottom.
 */
void printMatrixWithTotals(const std::vector<std::vector<int>>& mat, const MatrixSummary& summary) {
    int m = static_cast<int>(mat.size());
    int n = static_cast<int>(mat[0].size());

    std::cout << "      ";
    for (int j = 0; j < n; ++j) {
        std::cout << "Col " << std::setw(2) << j << " ";
    }
    std::cout << "|   Row Sum\n";

    std::cout << "     " << std::string(n * 8 + 14, '-') << '\n';

    for (int i = 0; i < m; ++i) {
        std::cout << "Row " << std::setw(2) << i << "|";
        for (int j = 0; j < n; ++j) {
            std::cout << std::setw(7) << mat[i][j] << " ";
        }
        std::cout << "| " << std::setw(8) << summary.rowSums[i] << '\n';
    }

    std::cout << "     " << std::string(n * 8 + 14, '-') << '\n';
    std::cout << "ColSum|";
    for (int j = 0; j < n; ++j) {
        std::cout << std::setw(7) << summary.colSums[j] << " ";
    }
    std::cout << "| " << std::setw(8) << summary.grandTotal << " (Total)\n";
}

int main() {
    std::cout << "=== Matrix Row Sum, Column Sum, and Grand Total Analysis ===\n\n";

    std::vector<std::vector<int>> matrix = {
        {12,  5,  9, 14,  3},
        { 8, 21,  4,  6, 17},
        {15,  7, 18, 11,  2},
        { 9, 13, 20,  8, 16}
    };

    auto summary = analyzeMatrixSums(matrix);

    printMatrixWithTotals(matrix, summary);

    std::cout << "\nSummary Insights:\n";
    std::cout << "  Grand Matrix Total:    " << summary.grandTotal << '\n';
    std::cout << "  Maximum Row Sum:       Row " << summary.maxRowIndex 
              << " with sum = " << summary.rowSums[summary.maxRowIndex] << '\n';
    std::cout << "  Maximum Column Sum:    Col " << summary.maxColIndex 
              << " with sum = " << summary.colSums[summary.maxColIndex] << '\n';

    return 0;
}
