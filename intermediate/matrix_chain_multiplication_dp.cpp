/*
 * Problem Description:
 * Matrix Chain Multiplication using Dynamic Programming.
 * Given a sequence of matrices, find the most efficient way to multiply them together.
 * The problem is to determine the optimal parenthesization that minimizes the total number
 * of scalar multiplications.
 * Example: For matrices A(10x30), B(30x5), C(5x60), the order of multiplication matters:
 * (AB)C = 10*30*5 + 10*5*60 = 1500 + 3000 = 4500
 * A(BC) = 30*5*60 + 10*30*60 = 9000 + 18000 = 27000
 * Time Complexity: O(N^3) where N is the number of matrices.
 * Space Complexity: O(N^2) for the DP table.
 */

#include <iostream>
#include <vector>
#include <climits>
#include <functional>
#include <string>

// Returns minimum number of scalar multiplications and the optimal parenthesization
std::pair<long long, std::string> matrixChainOrder(const std::vector<int>& dims) {
    int n = dims.size() - 1; // number of matrices
    // dp[i][j] = min cost to multiply matrices i through j
    std::vector<std::vector<long long>> dp(n, std::vector<long long>(n, 0));
    // split[i][j] = index k where optimal split occurs
    std::vector<std::vector<int>> split(n, std::vector<int>(n, 0));

    // Fill table for chains of increasing length
    for (int len = 2; len <= n; ++len) {
        for (int i = 0; i <= n - len; ++i) {
            int j = i + len - 1;
            dp[i][j] = LLONG_MAX;

            for (int k = i; k < j; ++k) {
                long long cost = dp[i][k] + dp[k + 1][j] +
                                 static_cast<long long>(dims[i]) * dims[k + 1] * dims[j + 1];
                if (cost < dp[i][j]) {
                    dp[i][j] = cost;
                    split[i][j] = k;
                }
            }
        }
    }

    // Reconstruct optimal parenthesization
    std::function<std::string(int, int)> buildParens = [&](int i, int j) -> std::string {
        if (i == j) {
            return "M" + std::to_string(i + 1);
        }
        return "(" + buildParens(i, split[i][j]) + " x " + buildParens(split[i][j] + 1, j) + ")";
    };

    return {dp[0][n - 1], buildParens(0, n - 1)};
}

int main() {
    // dims[i-1] x dims[i] gives dimensions of matrix i
    // Example: 4 matrices with dimensions 40x20, 20x30, 30x10, 10x30
    std::vector<int> dims = {40, 20, 30, 10, 30};

    auto [minCost, parens] = matrixChainOrder(dims);

    std::cout << "Matrix Chain Multiplication (Dynamic Programming)\n\n";
    std::cout << "Matrix dimensions:\n";
    for (size_t i = 0; i < dims.size() - 1; ++i) {
        std::cout << "  M" << (i + 1) << ": " << dims[i] << " x " << dims[i + 1] << "\n";
    }
    std::cout << "\nMinimum scalar multiplications: " << minCost << "\n";
    std::cout << "Optimal parenthesization: " << parens << "\n";

    // Second example
    std::vector<int> dims2 = {10, 30, 5, 60};
    auto [cost2, parens2] = matrixChainOrder(dims2);
    std::cout << "\nExample 2:\n";
    for (size_t i = 0; i < dims2.size() - 1; ++i) {
        std::cout << "  M" << (i + 1) << ": " << dims2[i] << " x " << dims2[i + 1] << "\n";
    }
    std::cout << "Min multiplications: " << cost2 << "\n";
    std::cout << "Optimal: " << parens2 << "\n";

    return 0;
}
