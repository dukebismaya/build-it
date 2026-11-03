/**
 * @file buzz_number_checker.cpp
 * @brief Checks if a number is a Buzz Number and finds Buzz numbers in a given range.
 *
 * Definition:
 * A Buzz Number is a positive integer that either:
 * 1. Ends with the digit 7 (i.e., N % 10 == 7), OR
 * 2. Is divisible by 7 (i.e., N % 7 == 0).
 *
 * Examples:
 * - 7:   Ends with 7 AND divisible by 7 (Buzz Number)
 * - 14:  Divisible by 7 (Buzz Number)
 * - 17:  Ends with 7 (Buzz Number)
 * - 28:  Divisible by 7 (Buzz Number)
 * - 49:  Ends with 7 AND divisible by 7 (Buzz Number)
 * - 52:  Neither ends with 7 nor divisible by 7 (Not a Buzz Number)
 *
 * Time Complexity:
 * - Single check: O(1)
 * - Range search up to N: O(N)
 * Space Complexity: O(1) auxiliary space
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

struct BuzzResult {
    bool isBuzz;
    bool endsWithSeven;
    bool divisibleBySeven;
};

/**
 * @brief Checks if an integer is a Buzz Number.
 * @param n Integer to check.
 * @return BuzzResult with detailed conditions.
 */
BuzzResult checkBuzzNumber(long long n) {
    if (n <= 0) return {false, false, false};

    bool ends7 = (n % 10 == 7);
    bool div7 = (n % 7 == 0);

    return {ends7 || div7, ends7, div7};
}

/**
 * @brief Generates all Buzz numbers up to a specified upper bound.
 */
std::vector<long long> findBuzzNumbers(long long limit) {
    std::vector<long long> results;
    for (long long i = 1; i <= limit; ++i) {
        if (checkBuzzNumber(i).isBuzz) {
            results.push_back(i);
        }
    }
    return results;
}

int main() {
    std::cout << "=== Buzz Number Checker & Range Generator ===\n\n";

    // 1. Evaluate specific test numbers
    std::vector<long long> testNumbers = {7, 14, 17, 21, 25, 27, 28, 35, 47, 49, 52, 70, 77};

    std::cout << "1. Verification of Selected Numbers:\n";
    std::cout << "  " << std::setw(8) << "Number"
              << std::setw(16) << "Ends with 7?"
              << std::setw(18) << "Divisible by 7?"
              << std::setw(16) << "Is Buzz Number?" << '\n';
    std::cout << "  " << std::string(58, '-') << '\n';

    for (long long num : testNumbers) {
        auto res = checkBuzzNumber(num);
        std::cout << "  " << std::setw(8) << num
                  << std::setw(16) << (res.endsWithSeven ? "YES" : "No")
                  << std::setw(18) << (res.divisibleBySeven ? "YES" : "No")
                  << std::setw(16) << (res.isBuzz ? "YES" : "No") << '\n';
    }

    // 2. Find all Buzz numbers up to 100
    std::cout << "\n2. All Buzz Numbers in range [1, 100]:\n  ";
    auto list = findBuzzNumbers(100);
    int count = 0;
    for (long long val : list) {
        std::cout << val << " ";
        count++;
        if (count % 10 == 0) std::cout << "\n  ";
    }
    std::cout << "\n\nTotal Buzz numbers in [1, 100]: " << list.size() << '\n';

    return 0;
}

