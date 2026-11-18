/**
 * ============================================================================
 * Bell Numbers & Bell Triangle (Aitken's Array / Set Partition Combinatorics)
 * ============================================================================
 * 
 * Description:
 * The n-th Bell number, denoted B(n), counts the total number of distinct
 * partitions of a set of n labeled elements into non-empty disjoint subsets.
 * 
 * First Bell Numbers (OEIS A000110):
 *   B(0) = 1, B(1) = 1, B(2) = 2, B(3) = 5, B(4) = 15, B(5) = 52,
 *   B(6) = 203, B(7) = 877, B(8) = 4140, B(9) = 21147, B(10) = 115975...
 * 
 * Generation via the Bell Triangle (Aitken's Array / Peirce Triangle):
 * The Bell triangle is constructed row by row:
 *   Row 0:  1
 *   Row 1:  1   2
 *   Row 2:  2   3   5
 *   Row 3:  5   7  10  15
 *   Row 4: 15  20  27  37  52
 * 
 * Construction Rules:
 * 1. The first element of row n is the last element of row n-1:
 *      A(n, 0) = A(n - 1, n - 1)
 * 2. Every subsequent element is the sum of the element to its left and the
 *    element directly above that element:
 *      A(n, k) = A(n, k - 1) + A(n - 1, k - 1)   for k >= 1.
 * 3. The last element of row n is B(n + 1).
 * 
 * Cross-Verification with Stirling Numbers of the Second Kind:
 *   B(n) = sum_{k=0}^n S(n, k)
 * where S(n, k) = k * S(n - 1, k) + S(n - 1, k - 1).
 * 
 * Time Complexity:
 * - Bell Triangle: O(N^2)
 * - Stirling Partition Sum: O(N^2)
 * - Space Complexity: O(N^2) for the triangle
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

namespace bell {

/**
 * Constructs the Bell Triangle up to row n and extracts Bell numbers B(0) .. B(n).
 */
std::vector<int64_t> generateBellNumbersTriangle(int n, std::vector<std::vector<int64_t>>& triangle_out) {
    std::vector<int64_t> b;
    b.push_back(1); // B(0) = 1
    if (n == 0) return b;

    triangle_out.resize(n);
    triangle_out[0].push_back(1); // Row 0: [1]
    b.push_back(1); // B(1) = 1

    for (int r = 1; r < n; ++r) {
        triangle_out[r].resize(r + 1);
        // Rule 1: A(r, 0) = A(r - 1, r - 1)
        triangle_out[r][0] = triangle_out[r - 1][r - 1];

        // Rule 2: A(r, k) = A(r, k - 1) + A(r - 1, k - 1)
        for (int k = 1; k <= r; ++k) {
            triangle_out[r][k] = triangle_out[r][k - 1] + triangle_out[r - 1][k - 1];
        }

        // The last element of row r is B(r + 1)
        b.push_back(triangle_out[r][r]);
    }

    return b;
}

/**
 * Computes Stirling numbers of the second kind S(n, k) and sums S(n, k) for k in [0, n].
 */
int64_t computeBellViaStirling(int n) {
    if (n == 0) return 1;

    // S[i][j] is the number of ways to partition i items into j subsets
    std::vector<std::vector<int64_t>> s(n + 1, std::vector<int64_t>(n + 1, 0));
    s[0][0] = 1;

    for (int i = 1; i <= n; ++i) {
        for (int k = 1; k <= i; ++k) {
            s[i][k] = k * s[i - 1][k] + s[i - 1][k - 1];
        }
    }

    int64_t total = 0;
    for (int k = 1; k <= n; ++k) {
        total += s[n][k];
    }
    return total;
}

} // namespace bell

int main() {
    std::cout << "========================================================\n";
    std::cout << " Bell Numbers & Aitken's Bell Triangle Generator\n";
    std::cout << "========================================================\n\n";

    constexpr int ROWS = 8;
    std::vector<std::vector<int64_t>> triangle;
    auto bell_nums = bell::generateBellNumbersTriangle(ROWS, triangle);

    std::cout << "Bell Triangle (First " << ROWS << " rows):\n";
    std::cout << "--------------------------------------------------------\n";
    for (int r = 0; r < ROWS; ++r) {
        std::cout << "  Row " << r << ": ";
        for (int k = 0; k <= r; ++k) {
            std::cout << std::setw(6) << triangle[r][k];
        }
        std::cout << "\n";
    }

    std::cout << "\n--------------------------------------------------------\n";
    std::cout << " Verification: Bell Triangle vs. Stirling Partition Sums\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << std::setw(4) << "n"
              << std::setw(16) << "B(n) [Triangle]"
              << std::setw(16) << "B(n) [Stirling]"
              << std::setw(14) << "Match?\n";
    std::cout << "--------------------------------------------------------\n";

    bool all_matched = true;
    for (int n = 0; n <= ROWS; ++n) {
        int64_t b_triangle = bell_nums[n];
        int64_t b_stirling = bell::computeBellViaStirling(n);

        std::cout << std::setw(4) << n
                  << std::setw(16) << b_triangle
                  << std::setw(16) << b_stirling
                  << std::setw(14) << (b_triangle == b_stirling ? "YES" : "MISMATCH")
                  << "\n";

        if (b_triangle != b_stirling) {
            all_matched = false;
        }
    }

    std::cout << "--------------------------------------------------------\n";
    std::cout << "Status: " << (all_matched ? "ALL TESTS PASSED!" : "FAILURE") << "\n";

    return 0;
}
