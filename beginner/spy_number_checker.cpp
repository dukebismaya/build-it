/**
 * @file spy_number_checker.cpp
 * @brief Checks if a number is a Spy Number and finds all Spy numbers in a range.
 *
 * Definition:
 * A number is called a Spy Number if the sum of all its digits is equal to
 * the product of all its digits.
 *
 * Examples:
 * - 123:   Sum = 1 + 2 + 3 = 6,   Product = 1 * 2 * 3 = 6       (Spy Number)
 * - 1124:  Sum = 1 + 1 + 2 + 4 = 8, Product = 1 * 1 * 2 * 4 = 8 (Spy Number)
 * - 22:    Sum = 2 + 2 = 4,       Product = 2 * 2 = 4           (Spy Number)
 * - 132:   Sum = 1 + 3 + 2 = 6,   Product = 1 * 3 * 2 = 6       (Spy Number)
 * - 124:   Sum = 1 + 2 + 4 = 7,   Product = 1 * 2 * 4 = 8 != 7  (Not a Spy Number)
 *
 * Time Complexity:
 * - Single check: O(log10(N))
 * - Range search up to N: O(N * log10(N))
 * Space Complexity: O(1) auxiliary space
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

struct SpyResult {
    bool isSpy;
    long long sumOfDigits;
    long long productOfDigits;
};

/**
 * @brief Checks whether a given non-negative integer is a Spy Number.
 * @param n Integer to check.
 * @return SpyResult containing verification status, sum, and product of digits.
 */
SpyResult checkSpyNumber(long long n) {
    if (n < 0) return {false, 0, 0};

    // Single digits 0-9
    if (n < 10) return {true, n, n};

    long long temp = n;
    long long sum = 0;
    long long product = 1;

    while (temp > 0) {
        long long digit = temp % 10;
        sum += digit;
        product *= digit;
        temp /= 10;
    }

    return {sum == product, sum, product};
}

/**
 * @brief Generates all Spy Numbers up to an upper bound.
 */
std::vector<long long> findSpyNumbers(long long limit) {
    std::vector<long long> results;
    for (long long i = 0; i <= limit; ++i) {
        if (checkSpyNumber(i).isSpy) {
            results.push_back(i);
        }
    }
    return results;
}

int main() {
    std::cout << "=== Spy Number Checker & Generator ===\n\n";

    // 1. Evaluate selected numbers
    std::vector<long long> testNumbers = {7, 22, 123, 124, 132, 213, 1124, 1133, 1234};

    std::cout << "1. Verification of Selected Numbers:\n";
    std::cout << "  " << std::setw(10) << "Number"
              << std::setw(16) << "Sum of Digits"
              << std::setw(18) << "Product of Digits"
              << std::setw(16) << "Is Spy Number?" << '\n';
    std::cout << "  " << std::string(60, '-') << '\n';

    for (long long num : testNumbers) {
        auto res = checkSpyNumber(num);
        std::cout << "  " << std::setw(10) << num
                  << std::setw(16) << res.sumOfDigits
                  << std::setw(18) << res.productOfDigits
                  << std::setw(16) << (res.isSpy ? "YES" : "No") << '\n';
    }

    // 2. Generate multi-digit spy numbers up to 1,500
    std::cout << "\n2. Multi-digit Spy Numbers up to 1,500:\n  ";
    auto list = findSpyNumbers(1500);
    int count = 0;
    for (long long val : list) {
        if (val >= 10) { // filter out single digits for display
            std::cout << val << " ";
            count++;
            if (count % 10 == 0) std::cout << "\n  ";
        }
    }
    std::cout << "\n\nTotal multi-digit Spy numbers found <= 1,500: " << count << '\n';

    return 0;
}

