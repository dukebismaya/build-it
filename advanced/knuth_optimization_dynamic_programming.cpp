/**
 * @file knuth_optimization_dynamic_programming.cpp
 * @brief Knuth's Dynamic Programming Optimization for Optimal BST in C++17.
 *
 * Overview (Donald Knuth, 1971):
 *   Accelerates interval dynamic programming recurrences of the form:
 *     dp[i][j] = min_{i <= k < j} { dp[i][k] + dp[k+1][j] } + cost(i, j)
 *   from standard cubic O(N^3) time down to quadratic O(N^2) time.
 *
 * Conditions for Knuth's Optimization:
 *   1. Quadrangle Inequality on cost:
 *        cost(a, c) + cost(b, d) <= cost(a, d) + cost(b, c)   for all a <= b <= c <= d.
 *   2. Monotonicity on subintervals:
 *        cost(b, c) <= cost(a, d)   for all [b, c] subset of [a, d].
 *
 * The Monotonicity of Optimal Split Points:
 *   Under these conditions, the optimal splitting index opt[i][j] satisfies:
 *     opt[i][j - 1] <= opt[i][j] <= opt[i + 1][j]
 *   Hence, the search range for k is restricted from [i, j-1] to [opt[i][j-1], opt[i+1][j]],
 *   which telescopes to a total of O(N^2) evaluations across all subproblems.
 *
 * Benchmark Problem:
 *   Optimal Binary Search Tree (OBST) given access frequencies w_1, w_2, ..., w_N.
 *
 * Complexity:
 *   - Time: O(N^2)
 *   - Space: O(N^2) for DP and opt tables.
 */

#include <iostream>
#include <vector>
#include <limits>
#include <iomanip>
#include <numeric>

namespace dp {

struct OBSTResult {
    int minSearchCost;
    std::vector<std::vector<int>> dp;
    std::vector<std::vector<int>> opt;
};

/**
 * @brief Solves Optimal Binary Search Tree in O(N^2) using Knuth's Optimization.
 * @param freq Access frequencies of sorted keys (1-based index).
 */
OBSTResult solveOptimalBST(const std::vector<int>& freq) {
    size_t n = freq.size();
    if (n == 0) return {0, {}, {}};

    // Prefix sums for fast range cost: sum(i, j) = freq[i] + ... + freq[j]
    std::vector<int> pref(n + 1, 0);
    for (size_t i = 0; i < n; ++i) {
        pref[i + 1] = pref[i] + freq[i];
    }
    auto rangeSum = [&](size_t i, size_t j) {
        return pref[j + 1] - pref[i];
    };

    const int INF = std::numeric_limits<int>::max() / 2;
    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));
    std::vector<std::vector<int>> opt(n, std::vector<int>(n, 0));

    // Base cases: single keys (length 1)
    for (size_t i = 0; i < n; ++i) {
        dp[i][i] = freq[i];
        opt[i][i] = static_cast<int>(i);
    }

    // Solve for lengths len = 2 to n
    for (size_t len = 2; len <= n; ++len) {
        for (size_t i = 0; i + len - 1 < n; ++i) {
            size_t j = i + len - 1;
            dp[i][j] = INF;

            // Knuth's bounded search range: opt[i][j-1] <= k <= opt[i+1][j]
            int kLow = opt[i][j - 1];
            int kHigh = opt[i + 1][j];

            for (int k = kLow; k <= kHigh; ++k) {
                int leftCost = (k > static_cast<int>(i)) ? dp[i][k - 1] : 0;
                int rightCost = (k < static_cast<int>(j)) ? dp[k + 1][j] : 0;
                int totalCost = leftCost + rightCost + rangeSum(i, j);

                if (totalCost < dp[i][j]) {
                    dp[i][j] = totalCost;
                    opt[i][j] = k;
                }
            }
        }
    }

    return {dp[0][n - 1], dp, opt};
}

} // namespace dp

int main() {
    std::cout << "========================================\n";
    std::cout << "  KNUTH'S DP OPTIMIZATION (O(N^2) OBST) \n";
    std::cout << " (Restricted Search by Quadrangle Ineq) \n";
    std::cout << "========================================\n\n";

    // Keys with access frequencies: [34, 8, 50, 10, 25]
    std::vector<int> frequencies = {34, 8, 50, 10, 25};
    size_t n = frequencies.size();

    std::cout << "Keys and Frequencies (" << n << " total):\n";
    for (size_t i = 0; i < n; ++i) {
        std::cout << "  Key " << (i + 1) << ": Weight = " << frequencies[i] << "\n";
    }
    std::cout << "\n";

    auto res = dp::solveOptimalBST(frequencies);

    std::cout << "Optimization Results:\n";
    std::cout << "  - Minimum Weighted Search Cost: " << res.minSearchCost << "\n";
    std::cout << "  - Optimal Root of entire tree : Key " << (res.opt[0][n - 1] + 1) << "\n\n";

    std::cout << "Optimal Split Points Matrix opt[i][j] (demonstrating monotonicity):\n";
    for (size_t i = 0; i < n; ++i) {
        std::cout << "  [ ";
        for (size_t j = 0; j < n; ++j) {
            if (i <= j) {
                std::cout << std::setw(3) << res.opt[i][j] << " ";
            } else {
                std::cout << "  - ";
            }
        }
        std::cout << "]\n";
    }

    std::cout << "\nComplexity: Reduced from cubic O(N^3) down to O(N^2).\n";
    std::cout << "Program finished successfully.\n";
    return 0;
}
