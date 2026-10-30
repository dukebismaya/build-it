/**
 * @file disarium_number_checker.cpp
 * @brief Checks if a number is a Disarium number and finds all Disarium numbers in a range.
 *
 * Definition:
 * A number is called a Disarium number if the sum of its digits powered with their
 * respective positions (1-indexed from left to right) is equal to the number itself.
 *
 * Examples:
 * - 89:   8^1 + 9^2 = 8 + 81 = 89 (Disarium)
 * - 135:  1^1 + 3^2 + 5^3 = 1 + 9 + 125 = 135 (Disarium)
 * - 175:  1^1 + 7^2 + 5^3 = 1 + 49 + 125 = 175 (Disarium)
 * - 518:  5^1 + 1^2 + 8^3 = 5 + 1 + 512 = 518 (Disarium)
 * - 598:  5^1 + 9^2 + 8^3 = 5 + 81 + 512 = 598 (Disarium)
 *
 * Time Complexity:
 * - Single number check: O(d) where d is number of digits (d <= 19 for 64-bit integers)
 * - Range search up to N: O(N * log10(N))
 * Space Complexity: O(d) to store digits.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <iomanip>

/**
 * @brief Computes base^exp for non-negative 64-bit integers.
 */
long long integerPower(long long base, int exp) {
    long long res = 1;
    long long b = base;
    while (exp > 0) {
        if (exp & 1) res *= b;
        b *= b;
        exp >>= 1;
    }
    return res;
}

/**
 * @brief Checks whether a given positive integer is a Disarium number.
 * @param n Positive integer to check.
 * @return True if n is Disarium, false otherwise.
 */
bool isDisarium(long long n) {
    if (n <= 0) return false;

    // Extract digits
    std::vector<int> digits;
    long long temp = n;
    while (temp > 0) {
        digits.push_back(static_cast<int>(temp % 10));
        temp /= 10;
    }

    // Digits are in reverse order (units first)
    int len = static_cast<int>(digits.size());
    long long sumOfPowers = 0;

    for (int i = 0; i < len; ++i) {
        int pos = len - i; // Position from left: 1, 2, ..., len
        sumOfPowers += integerPower(digits[i], pos);
    }

    return sumOfPowers == n;
}

/**
 * @brief Finds all Disarium numbers up to a specified upper bound.
 * @param limit Upper bound inclusive.
 * @return Vector containing all Disarium numbers in [1, limit].
 */
std::vector<long long> findDisariumNumbers(long long limit) {
    std::vector<long long> results;
    for (long long i = 1; i <= limit; ++i) {
        if (isDisarium(i)) {
            results.push_back(i);
        }
    }
    return results;
}

int main() {
    std::cout << "=== Disarium Number Checker & Generator ===\n\n";

    // 1. Check specific numbers
    std::vector<long long> testCases = {80, 89, 135, 175, 200, 518, 598, 1306, 1676, 2427};

    std::cout << "1. Checking Selected Numbers:\n";
    std::cout << "  " << std::setw(10) << "Number"
              << std::setw(18) << "Disarium?"
              << std::setw(30) << "Digit Power Expansion" << '\n';
    std::cout << "  " << std::string(58, '-') << '\n';

    for (long long val : testCases) {
        bool result = isDisarium(val);

        // Build formula string
        std::string s = std::to_string(val);
        std::string formula;
        long long sum = 0;
        for (size_t i = 0; i < s.length(); ++i) {
            int digit = s[i] - '0';
            int pos = static_cast<int>(i + 1);
            long long p = integerPower(digit, pos);
            sum += p;
            formula += std::to_string(digit) + "^" + std::to_string(pos);
            if (i + 1 < s.length()) formula += " + ";
        }
        formula += " = " + std::to_string(sum);

        std::cout << "  " << std::setw(10) << val
                  << std::setw(18) << (result ? "YES (Disarium)" : "No")
                  << "   " << formula << '\n';
    }

    // 2. Range search up to 2,000
    std::cout << "\n2. All Disarium numbers up to 2,000:\n  ";
    auto list = findDisariumNumbers(2000);
    for (size_t i = 0; i < list.size(); ++i) {
        std::cout << list[i];
        if (i + 1 < list.size()) std::cout << ", ";
    }
    std::cout << "\n\nTotal found in range [1, 2000]: " << list.size() << '\n';

    return 0;
}
