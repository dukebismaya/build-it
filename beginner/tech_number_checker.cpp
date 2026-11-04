/**
 * @file tech_number_checker.cpp
 * @brief Checks if a number is a Tech Number and generates Tech numbers in a range.
 *
 * Definition:
 * A Tech Number is a number that has an even number of decimal digits 2d, such that
 * when it is split into two equal halves of d digits each, the square of the sum
 * of these two halves is equal to the original number itself:
 *   (FirstHalf + SecondHalf)^2 == N
 *
 * Examples:
 * - 2025:
 *   Digits = 4 (even, d = 2)
 *   First half = 20, Second half = 25
 *   Sum = 20 + 25 = 45
 *   45^2 = 2025 (Tech Number)
 *
 * - 3025:
 *   First half = 30, Second half = 25
 *   Sum = 30 + 25 = 55
 *   55^2 = 3025 (Tech Number)
 *
 * - 9801:
 *   First half = 98, Second half = 01 = 1
 *   Sum = 98 + 1 = 99
 *   99^2 = 9801 (Tech Number)
 *
 * - 81:
 *   Digits = 2 (even, d = 1)
 *   First half = 8, Second half = 1
 *   Sum = 8 + 1 = 9
 *   9^2 = 81 (Tech Number)
 *
 * Time Complexity:
 * - Single check: O(log10(N))
 * - Range search up to N: O(N * log10(N))
 * Space Complexity: O(1) auxiliary space
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

struct TechResult {
    bool isTech;
    long long firstHalf;
    long long secondHalf;
    long long sumOfHalves;
    std::string reason;
};

/**
 * @brief Checks whether a given positive integer is a Tech Number.
 * @param n Integer to check.
 * @return TechResult with verification details.
 */
TechResult checkTechNumber(long long n) {
    if (n <= 0) {
        return {false, 0, 0, 0, "Must be positive"};
    }

    std::string s = std::to_string(n);
    int numDigits = static_cast<int>(s.length());

    if (numDigits % 2 != 0) {
        return {false, 0, 0, 0, "Number of digits is odd (" + std::to_string(numDigits) + ")"};
    }

    int halfDigits = numDigits / 2;
    long long divisor = 1;
    for (int i = 0; i < halfDigits; ++i) {
        divisor *= 10;
    }

    long long firstHalf = n / divisor;
    long long secondHalf = n % divisor;
    long long sum = firstHalf + secondHalf;
    long long sq = sum * sum;

    if (sq == n) {
        return {true, firstHalf, secondHalf, sum,
                "(" + std::to_string(firstHalf) + " + " + std::to_string(secondHalf) + ")^2 = " +
                std::to_string(sum) + "^2 = " + std::to_string(n)};
    }

    return {false, firstHalf, secondHalf, sum,
            "(" + std::to_string(firstHalf) + " + " + std::to_string(secondHalf) + ")^2 = " +
            std::to_string(sq) + " != " + std::to_string(n)};
}

/**
 * @brief Generates all Tech numbers up to an upper bound.
 */
std::vector<long long> findTechNumbers(long long limit) {
    std::vector<long long> results;
    for (long long i = 1; i <= limit; ++i) {
        if (checkTechNumber(i).isTech) {
            results.push_back(i);
        }
    }
    return results;
}

int main() {
    std::cout << "=== Tech Number Checker & Generator ===\n\n";

    // 1. Evaluate selected numbers
    std::vector<long long> testCandidates = {81, 100, 2025, 3025, 5500, 9801, 12345, 100000};

    std::cout << "1. Verification of Selected Numbers:\n";
    std::cout << "  " << std::setw(8) << "Number"
              << std::setw(14) << "Is Tech?"
              << "   Details\n";
    std::cout << "  " << std::string(58, '-') << '\n';

    for (long long num : testCandidates) {
        auto res = checkTechNumber(num);
        std::cout << "  " << std::setw(8) << num
                  << std::setw(14) << (res.isTech ? "YES" : "No")
                  << "   " << res.reason << '\n';
    }

    // 2. Find all 4-digit Tech numbers (1000 to 9999)
    std::cout << "\n2. All 4-digit Tech Numbers (1000 to 9999):\n";
    auto list4 = findTechNumbers(9999);
    for (long long val : list4) {
        if (val >= 1000) {
            auto r = checkTechNumber(val);
            std::cout << "  " << val << ": " << r.reason << '\n';
        }
    }

    return 0;
}
