/**
 * @file sublime_number_checker.cpp
 * @brief Analysis and verification of Sublime Numbers in C++17.
 *
 * Mathematical Definition:
 *   A sublime number is a positive integer n such that:
 *     1. The total number of positive divisors d(n) is a perfect number.
 *     2. The sum of all positive divisors sigma(n) is also a perfect number.
 *
 * Perfect Number Recall:
 *   A number P is perfect if the sum of its proper divisors equals P,
 *   i.e., sigma(P) = 2 * P.
 *   Examples of perfect numbers: 6, 28, 496, 8128, 33550336, ...
 *
 * The Smallest Sublime Number (12):
 *   - Divisors of 12: {1, 2, 3, 4, 6, 12}
 *   - Number of divisors: d(12) = 6  (which is perfect: 1 + 2 + 3 = 6)
 *   - Sum of divisors: sigma(12) = 1 + 2 + 3 + 4 + 6 + 12 = 28  (which is perfect: 1 + 2 + 4 + 7 + 14 = 28)
 *   Thus, 12 is sublime!
 *
 * Historical Note:
 *   Only two sublime numbers are known: 12, and a 76-digit number found by
 *   Kevin Brown in 1996: 2^126 * (2^61 - 1) * (2^31 - 1) * (2^19 - 1) * (2^7 - 1) * (2^5 - 1) * (2^3 - 1).
 *
 * Complexity:
 *   - Divisor enumeration & sum: O(sqrt(n)).
 *   - Perfect number verification: O(sqrt(d(n)) + sqrt(sigma(n))).
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>

namespace number_theory {

/**
 * @brief Checks if a positive integer is a perfect number: sigma(n) == 2 * n.
 */
bool isPerfect(uint64_t n) {
    if (n <= 1) return false;
    uint64_t sum = 1;
    for (uint64_t d = 2; d * d <= n; ++d) {
        if (n % d == 0) {
            sum += d;
            uint64_t other = n / d;
            if (other != d) {
                sum += other;
            }
        }
    }
    return (sum == n);
}

struct SublimeDetails {
    uint64_t n;
    uint64_t divisorCount;
    uint64_t divisorSum;
    bool isCountPerfect;
    bool isSumPerfect;
    bool isSublime;
};

/**
 * @brief Evaluates if n is a sublime number.
 */
SublimeDetails evaluateSublime(uint64_t n) {
    if (n == 0) return {0, 0, 0, false, false, false};

    uint64_t count = 0;
    uint64_t sum = 0;

    for (uint64_t d = 1; d * d <= n; ++d) {
        if (n % d == 0) {
            count++;
            sum += d;
            uint64_t other = n / d;
            if (other != d) {
                count++;
                sum += other;
            }
        }
    }

    bool countPerf = isPerfect(count);
    bool sumPerf = isPerfect(sum);

    return {n, count, sum, countPerf, sumPerf, (countPerf && sumPerf)};
}

} // namespace number_theory

int main() {
    std::cout << "========================================\n";
    std::cout << "        SUBLIME NUMBER CHECKER          \n";
    std::cout << " (d(n) is Perfect AND sigma(n) is Perf) \n";
    std::cout << "========================================\n\n";

    std::vector<uint64_t> testValues = {6, 12, 18, 24, 28, 36, 48, 60};

    std::cout << std::left << std::setw(6)  << "n"
              << std::setw(12) << "d(n)"
              << std::setw(16) << "d(n) Perfect?"
              << std::setw(14) << "sigma(n)"
              << std::setw(16) << "sigma(n) Perf?"
              << "Is Sublime?\n";
    std::cout << std::string(75, '-') << "\n";

    for (uint64_t val : testValues) {
        auto res = number_theory::evaluateSublime(val);

        std::cout << std::left << std::setw(6)  << val
                  << std::setw(12) << res.divisorCount
                  << std::setw(16) << (res.isCountPerfect ? "YES" : "No")
                  << std::setw(14) << res.divisorSum
                  << std::setw(16) << (res.isSumPerfect ? "YES" : "No")
                  << (res.isSublime ? "YES (Sublime!)" : "No") << "\n";
    }

    std::cout << "\nDetailed Verification for n = 12:\n";
    std::cout << "  - Divisors: {1, 2, 3, 4, 6, 12}\n";
    std::cout << "  - Divisor Count d(12) = 6 = 1 + 2 + 3 (1st Perfect Number)\n";
    std::cout << "  - Divisor Sum sigma(12) = 28 = 1 + 2 + 4 + 7 + 14 (2nd Perfect Number)\n";
    std::cout << "  -> Both criteria satisfied identically.\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
