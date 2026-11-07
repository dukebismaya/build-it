/**
 * @file catalan_number_generator.cpp
 * @brief Computation of Catalan Numbers and combinatorial problem applications in C++17.
 *
 * Catalan numbers C_n form a sequence of natural numbers with widespread applications
 * in combinatorial mathematics. The nth Catalan number is given by:
 *   C_n = (1 / (n + 1)) * binom(2n, n) = (2n)! / ((n + 1)! * n!)
 *
 * Direct recurrence relation:
 *   C_0 = 1
 *   C_n = (2 * (2n - 1) / (n + 1)) * C_{n-1}
 *
 * Classic combinatorial interpretations:
 *   1. Number of valid Dyck paths / expressions with n pairs of balanced parentheses.
 *   2. Number of full binary trees with (n + 1) leaves (n internal nodes).
 *   3. Number of ways to triangulate a convex polygon with (n + 2) sides.
 *   4. Number of monotonic lattice paths along edges of an n x n grid that do not cross the diagonal.
 *
 * Complexity:
 *   - Iterative multiplicative generation of C_n: O(n) time, O(1) space.
 *   - Dynamic programming convolution table: O(n^2) time, O(n) space.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <string>

namespace combinatorics {

/**
 * @brief Computes the nth Catalan number directly using iterative multiplication.
 * @param n Index (0 <= n <= 33 for 64-bit unsigned integers).
 * @return uint64_t representing C_n.
 */
uint64_t computeCatalanDirect(int n) {
    if (n < 0) return 0;
    if (n == 0 || n == 1) return 1;

    uint64_t c = 1;
    for (int i = 1; i <= n; ++i) {
        c = (c * 2 * (2 * i - 1)) / (i + 1);
    }
    return c;
}

/**
 * @brief Computes the first N Catalan numbers using dynamic programming convolution:
 *        C_0 = 1, C_n = sum_{i=0}^{n-1} C_i * C_{n-1-i}
 * @param n Maximum index.
 * @return std::vector<uint64_t> containing C_0 through C_n.
 */
std::vector<uint64_t> computeCatalanDP(int n) {
    if (n < 0) return {};
    std::vector<uint64_t> dp(n + 1, 0);
    dp[0] = 1;

    for (int i = 1; i <= n; ++i) {
        for (int j = 0; j < i; ++j) {
            dp[i] += dp[j] * dp[i - 1 - j];
        }
    }
    return dp;
}

/**
 * @brief Generates all valid parentheses strings for n pairs.
 * @param n Number of pairs.
 * @return Vector of valid parentheses strings.
 */
void generateParenthesesHelper(int openRem, int closeRem, std::string current, std::vector<std::string>& results) {
    if (openRem == 0 && closeRem == 0) {
        results.push_back(current);
        return;
    }
    if (openRem > 0) {
        generateParenthesesHelper(openRem - 1, closeRem, current + "(", results);
    }
    if (closeRem > openRem) {
        generateParenthesesHelper(openRem, closeRem - 1, current + ")", results);
    }
}

std::vector<std::string> generateBalancedParentheses(int n) {
    std::vector<std::string> results;
    generateParenthesesHelper(n, n, "", results);
    return results;
}

} // namespace combinatorics

int main() {
    std::cout << "========================================\n";
    std::cout << "       CATALAN NUMBER GENERATOR         \n";
    std::cout << "========================================\n\n";

    int limit = 15;
    auto dpTable = combinatorics::computeCatalanDP(limit);

    std::cout << "First " << limit + 1 << " Catalan Numbers (C_0 to C_" << limit << "):\n";
    std::cout << std::left << std::setw(6) << "n"
              << std::setw(20) << "C_n (Direct)"
              << std::setw(20) << "C_n (Convolution DP)"
              << "\n";
    std::cout << std::string(46, '-') << "\n";

    for (int i = 0; i <= limit; ++i) {
        uint64_t direct = combinatorics::computeCatalanDirect(i);
        std::cout << std::left << std::setw(6) << i
                  << std::setw(20) << direct
                  << std::setw(20) << dpTable[i]
                  << "\n";
    }

    std::cout << "\nCombinatorial Application: Balanced Parentheses Combinations for n = 3:\n";
    auto parens3 = combinatorics::generateBalancedParentheses(3);
    std::cout << "C_3 = " << parens3.size() << " combinations:\n";
    for (size_t i = 0; i < parens3.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << parens3[i] << "\n";
    }

    std::cout << "\nProgram finished successfully.\n";
    return 0;
}
