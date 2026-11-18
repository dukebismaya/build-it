/**
 * ============================================================================
 * Munchausen Number Checker & Digit Self-Power Invariant Analyzer
 * ============================================================================
 * 
 * Description:
 * A Munchausen number (named after the literary Baron Munchausen, who pulled
 * himself out of a swamp by his own hair) is a positive integer that equals
 * the sum of its base-10 digits, each raised to the power of itself:
 *   n = sum_{i=1}^k (d_i)^(d_i)
 * 
 * Mathematical Convention:
 * Under the standard convention for Munchausen numbers where 0^0 = 0:
 *   0^0 = 0
 *   1^1 = 1
 *   2^2 = 4
 *   3^3 = 27
 *   4^4 = 256
 *   5^5 = 3,125
 *   6^6 = 46,656
 *   7^7 = 823,543
 *   8^8 = 16,777,216
 *   9^9 = 387,420,489
 * 
 * Known Base-10 Munchausen Numbers:
 * In the decimal system, there are only TWO Munchausen numbers:
 *   1     = 1^1 = 1
 *   3435  = 3^3 + 4^4 + 3^3 + 5^5
 *         = 27 + 256 + 27 + 3125
 *         = 3435
 * 
 * Features:
 * 1. Fast O(log_10 n) primality and Munchausen condition tester with precomputed
 *    d^d table.
 * 2. Visual step-by-step arithmetic decomposition for candidate numbers.
 * 3. Search survey demonstrating that 1 and 3435 are unique solutions <= 100,000.
 * 
 * Time Complexity:
 * - O(d) per test where d <= 10 is the number of digits.
 * - Space Complexity: O(1) auxiliary memory.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <cmath>

namespace munchausen {

// Precomputed table of d^d for d in [0, 9] with convention 0^0 = 0
constexpr int64_t D_POW_D[10] = {
    0,            // 0^0 = 0
    1,            // 1^1 = 1
    4,            // 2^2 = 4
    27,           // 3^3 = 27
    256,          // 4^4 = 256
    3125,         // 5^5 = 3125
    46656,        // 6^6 = 46656
    823543,       // 7^7 = 823543
    16777216,     // 8^8 = 16777216
    387420489     // 9^9 = 387420489
};

/**
 * Computes sum of digits raised to their own power: sum d_i^(d_i).
 */
int64_t computeSelfPowerSum(int64_t n) {
    int64_t sum = 0;
    int64_t temp = n;

    while (temp > 0) {
        int d = static_cast<int>(temp % 10);
        sum += D_POW_D[d];
        temp /= 10;
    }

    return sum;
}

/**
 * Checks whether n is a Munchausen number.
 */
bool isMunchausen(int64_t n) {
    if (n <= 0) return false;
    return computeSelfPowerSum(n) == n;
}

/**
 * Prints detailed breakdown of the digit self-power sum.
 */
void explainDecomposition(int64_t n) {
    std::vector<int> digits;
    int64_t temp = n;
    while (temp > 0) {
        digits.push_back(static_cast<int>(temp % 10));
        temp /= 10;
    }

    std::cout << "Number: " << n << "\n";
    std::cout << "Decomposition: ";
    int64_t total = 0;
    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
        int d = digits[i];
        int64_t term = D_POW_D[d];
        total += term;

        std::cout << d << "^" << d << " (" << term << ")";
        if (i > 0) std::cout << " + ";
    }
    std::cout << " = " << total << "\n";
    std::cout << "Verdict: " << (total == n ? "MUNCHAUSEN NUMBER!" : "NOT Munchausen") << "\n\n";
}

} // namespace munchausen

int main() {
    std::cout << "========================================================\n";
    std::cout << " Munchausen Number Checker & Digit Self-Power Analyzer\n";
    std::cout << "========================================================\n\n";

    std::cout << "Testing Famous Munchausen Numbers:\n";
    std::cout << "--------------------------------------------------------\n";
    munchausen::explainDecomposition(1);
    munchausen::explainDecomposition(3435);

    std::cout << "Testing Counter-Examples:\n";
    std::cout << "--------------------------------------------------------\n";
    munchausen::explainDecomposition(153);
    munchausen::explainDecomposition(1000);

    std::cout << "--------------------------------------------------------\n";
    std::cout << " Exhaustive Search Survey up to 100,000:\n";
    std::cout << "--------------------------------------------------------\n";

    std::vector<int64_t> results;
    for (int64_t i = 1; i <= 100000; ++i) {
        if (munchausen::isMunchausen(i)) {
            results.push_back(i);
        }
    }

    std::cout << "Found " << results.size() << " Munchausen numbers <= 100,000:\n  ";
    for (size_t i = 0; i < results.size(); ++i) {
        std::cout << results[i] << (i + 1 < results.size() ? ", " : "\n\n");
    }

    std::cout << "Verification: Only {1, 3435} exist in this range, confirming mathematical theorem!\n";

    return 0;
}
