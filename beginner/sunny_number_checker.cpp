/**
 * @file sunny_number_checker.cpp
 * @brief Checks if a number is a Sunny Number and generates Sunny numbers in a range.
 *
 * Definition:
 * A number N is called a Sunny Number if N + 1 is a perfect square.
 * That is, there exists an integer k >= 1 such that:
 *   N + 1 = k^2  <=>  N = k^2 - 1
 *
 * Examples:
 * - 3:  3 + 1 = 4 = 2^2   (Sunny Number)
 * - 8:  8 + 1 = 9 = 3^2   (Sunny Number)
 * - 15: 15 + 1 = 16 = 4^2 (Sunny Number)
 * - 24: 24 + 1 = 25 = 5^2 (Sunny Number)
 * - 35: 35 + 1 = 36 = 6^2 (Sunny Number)
 * - 80: 80 + 1 = 81 = 9^2 (Sunny Number)
 * - 10: 10 + 1 = 11 (Not a perfect square -> Not a Sunny Number)
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

struct SunnyResult {
    bool isSunny;
    long long root;
};

/**
 * @brief Checks if a given non-negative integer is a Sunny Number.
 * @param n Integer to check.
 * @return SunnyResult indicating status and integer root k where k^2 = n + 1.
 */
SunnyResult checkSunny(long long n) {
    if (n < 0) return {false, -1};

    long long target = n + 1;
    long long root = static_cast<long long>(std::round(std::sqrt(target)));

    if (root * root == target) {
        return {true, root};
    }

    return {false, -1};
}

/**
 * @brief Generates all Sunny Numbers up to an upper bound.
 * @param limit Upper bound inclusive.
 * @return Vector of pairs (k, SunnyNumber) where SunnyNumber = k^2 - 1.
 */
std::vector<std::pair<long long, long long>> generateSunnyNumbers(long long limit) {
    std::vector<std::pair<long long, long long>> results;
    for (long long k = 1; (k * k - 1) <= limit; ++k) {
        results.push_back({k, k * k - 1});
    }
    return results;
}

int main() {
    std::cout << "=== Sunny Number Checker & Generator ===\n\n";

    // 1. Evaluate selected numbers
    std::vector<long long> testNumbers = {0, 3, 5, 8, 10, 15, 20, 24, 35, 48, 50, 80, 99};

    std::cout << "1. Verification of Selected Numbers:\n";
    std::cout << "  " << std::setw(8) << "Number"
              << std::setw(14) << "N + 1"
              << std::setw(16) << "Is Sunny?"
              << std::setw(20) << "Perfect Square Form" << '\n';
    std::cout << "  " << std::string(60, '-') << '\n';

    for (long long num : testNumbers) {
        auto res = checkSunny(num);
        std::cout << "  " << std::setw(8) << num
                  << std::setw(14) << (num + 1)
                  << std::setw(16) << (res.isSunny ? "YES" : "No");
        if (res.isSunny) {
            std::cout << "   " << num << " + 1 = " << res.root << "^2\n";
        } else {
            std::cout << std::setw(20) << "-" << '\n';
        }
    }

    // 2. Generate all Sunny numbers up to 200
    std::cout << "\n2. All Sunny Numbers <= 200:\n";
    std::cout << "  " << std::setw(6) << "k"
              << std::setw(16) << "Sunny Number"
              << std::setw(16) << "Formula (k^2 - 1)" << '\n';
    std::cout << "  " << std::string(40, '-') << '\n';

    auto sunnies = generateSunnyNumbers(200);
    for (const auto& [k, val] : sunnies) {
        std::cout << "  " << std::setw(6) << k
                  << std::setw(16) << val
                  << "   " << k << "^2 - 1 = " << val << '\n';
    }
    std::cout << "\nTotal Sunny numbers found <= 200: " << sunnies.size() << '\n';

    return 0;
}

