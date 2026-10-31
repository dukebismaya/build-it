/**
 * @file pronic_number_checker.cpp
 * @brief Checks if a number is a Pronic (oblong) number and finds pronic numbers in a range.
 *
 * Definition:
 * A Pronic number (also called an oblong number or heteromecic number) is a number
 * that is the product of two consecutive non-negative integers:
 *   N = k * (k + 1)   for some integer k >= 0.
 *
 * Sequence of first few pronic numbers:
 *   k = 0  -> 0 * 1 = 0
 *   k = 1  -> 1 * 2 = 2
 *   k = 2  -> 2 * 3 = 6
 *   k = 3  -> 3 * 4 = 12
 *   k = 4  -> 4 * 5 = 20
 *   k = 5  -> 5 * 6 = 30
 *   k = 6  -> 6 * 7 = 42
 *   k = 7  -> 7 * 8 = 56
 *   k = 8  -> 8 * 9 = 72
 *   k = 9  -> 9 * 10 = 90
 *
 * Algorithm:
 * For a given N >= 0, let k = floor(sqrt(N)).
 * If k * (k + 1) == N, then N is pronic with consecutive factors k and k + 1.
 *
 * Time Complexity:
 * - Single check: O(1)
 * - Range generation up to limit L: O(sqrt(L))
 * Space Complexity: O(1) auxiliary space
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

struct PronicResult {
    bool isPronic;
    long long factorK;
    long long factorKPlus1;
};

/**
 * @brief Checks whether a given non-negative integer is a Pronic number.
 * @param n Integer to check.
 * @return PronicResult with verification status and consecutive factors.
 */
PronicResult checkPronic(long long n) {
    if (n < 0) return {false, -1, -1};
    if (n == 0) return {true, 0, 1};

    long long k = static_cast<long long>(std::sqrt(n));
    if (k * (k + 1) == n) {
        return {true, k, k + 1};
    }
    // Handle floating-point edge case around sqrt
    if ((k - 1) * k == n) {
        return {true, k - 1, k};
    }

    return {false, -1, -1};
}

/**
 * @brief Generates all Pronic numbers up to an upper bound.
 * @param limit Maximum value inclusive.
 * @return Vector of pairs (k, PronicNumber).
 */
std::vector<std::pair<long long, long long>> generatePronicNumbers(long long limit) {
    std::vector<std::pair<long long, long long>> results;
    for (long long k = 0; k * (k + 1) <= limit; ++k) {
        results.push_back({k, k * (k + 1)});
    }
    return results;
}

int main() {
    std::cout << "=== Pronic (Oblong / Heteromecic) Number Checker ===\n\n";

    // 1. Evaluate selected numbers
    std::vector<long long> testNumbers = {0, 2, 6, 10, 12, 20, 25, 42, 56, 70, 72, 90, 110, 132, 1000};

    std::cout << "1. Verification of Selected Numbers:\n";
    std::cout << "  " << std::setw(10) << "Number"
              << std::setw(16) << "Is Pronic?"
              << std::setw(24) << "Consecutive Product" << '\n';
    std::cout << "  " << std::string(50, '-') << '\n';

    for (long long num : testNumbers) {
        auto res = checkPronic(num);
        std::cout << "  " << std::setw(10) << num
                  << std::setw(16) << (res.isPronic ? "YES" : "No");
        if (res.isPronic) {
            std::cout << "   " << res.factorK << " * " << res.factorKPlus1
                      << " = " << num << '\n';
        } else {
            std::cout << std::setw(24) << "-" << '\n';
        }
    }

    // 2. Generate all pronic numbers up to 300
    std::cout << "\n2. All Pronic Numbers <= 300:\n";
    std::cout << "  " << std::setw(6) << "k"
              << std::setw(16) << "Pronic Number"
              << std::setw(16) << "Formula k*(k+1)" << '\n';
    std::cout << "  " << std::string(38, '-') << '\n';

    auto pronics = generatePronicNumbers(300);
    for (const auto& [k, val] : pronics) {
        std::cout << "  " << std::setw(6) << k
                  << std::setw(16) << val
                  << std::setw(12) << k << " * " << (k + 1) << '\n';
    }
    std::cout << "\nTotal Pronic numbers found <= 300: " << pronics.size() << '\n';

    return 0;
}
