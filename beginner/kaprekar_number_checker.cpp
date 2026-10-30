/**
 * @file kaprekar_number_checker.cpp
 * @brief Checks if a number is a Kaprekar number and generates Kaprekar numbers in a range.
 *
 * Definition:
 * A positive integer X is called a Kaprekar number (in base 10) if the representation
 * of its square X^2 can be split into two parts that add up to X, where the second part
 * is positive (cannot be all zeros).
 *
 * Formal Rule:
 * Let X^2 have d digits. For any split into left part L and right part R such that
 * X^2 = L * 10^k + R (where 10^k > R and R > 0):
 * If L + R = X, then X is a Kaprekar number.
 * Note: 1 is considered a Kaprekar number because 1^2 = 1, split as L = 0, R = 1 -> 0 + 1 = 1.
 *
 * Examples:
 * - 9:    9^2 = 81       -> 8 + 1 = 9
 * - 45:   45^2 = 2025    -> 20 + 25 = 45
 * - 55:   55^2 = 3025    -> 30 + 25 = 55
 * - 99:   99^2 = 9801    -> 98 + 01 = 99
 * - 297:  297^2 = 88209  -> 88 + 209 = 297
 * - 703:  703^2 = 494209 -> 494 + 209 = 703
 *
 * Time Complexity:
 * - Single check: O(log10(X^2))
 * - Range search up to N: O(N * log10(N))
 * Space Complexity: O(1) auxiliary space
 */

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

struct KaprekarSplit {
    bool isKaprekar;
    long long left;
    long long right;
    long long square;
};

/**
 * @brief Checks if a given positive integer is a Kaprekar number.
 * @param n Positive integer to check.
 * @return KaprekarSplit structure containing verification details.
 */
KaprekarSplit checkKaprekar(long long n) {
    if (n <= 0) return {false, 0, 0, 0};
    if (n == 1) return {true, 0, 1, 1};

    long long sq = n * n;
    long long divisor = 10;

    while (divisor <= sq) {
        // Avoid division by 10 resulting in right part having multiple splits
        long long right = sq % divisor;
        long long left = sq / divisor;

        if (right > 0 && left + right == n) {
            return {true, left, right, sq};
        }

        // Avoid overflow on divisor
        if (divisor > sq / 10) break;
        divisor *= 10;
    }

    return {false, 0, 0, sq};
}

/**
 * @brief Finds all Kaprekar numbers up to a specified upper bound.
 * @param limit Upper bound inclusive.
 * @return Vector of Kaprekar numbers.
 */
std::vector<long long> findKaprekarNumbers(long long limit) {
    std::vector<long long> results;
    for (long long i = 1; i <= limit; ++i) {
        if (checkKaprekar(i).isKaprekar) {
            results.push_back(i);
        }
    }
    return results;
}

int main() {
    std::cout << "=== Kaprekar Number Checker & Generator ===\n\n";

    // 1. Check selected candidates
    std::vector<long long> testNumbers = {1, 9, 10, 45, 55, 99, 100, 297, 703, 999, 2223};

    std::cout << "1. Verification of Selected Numbers:\n";
    std::cout << "  " << std::setw(8) << "Number"
              << std::setw(14) << "Square"
              << std::setw(14) << "Left Part"
              << std::setw(14) << "Right Part"
              << std::setw(16) << "Is Kaprekar?" << '\n';
    std::cout << "  " << std::string(66, '-') << '\n';

    for (long long num : testNumbers) {
        auto res = checkKaprekar(num);
        std::cout << "  " << std::setw(8) << num
                  << std::setw(14) << res.square;
        if (res.isKaprekar) {
            std::cout << std::setw(14) << res.left
                      << std::setw(14) << res.right
                      << std::setw(16) << "YES" << '\n';
        } else {
            std::cout << std::setw(14) << "-"
                      << std::setw(14) << "-"
                      << std::setw(16) << "No" << '\n';
        }
    }

    // 2. Range search up to 10,000
    std::cout << "\n2. All Kaprekar Numbers up to 10,000:\n  ";
    auto list = findKaprekarNumbers(10000);
    for (size_t i = 0; i < list.size(); ++i) {
        std::cout << list[i];
        if (i + 1 < list.size()) std::cout << ", ";
    }
    std::cout << "\n\nTotal found in range [1, 10000]: " << list.size() << '\n';

    return 0;
}
