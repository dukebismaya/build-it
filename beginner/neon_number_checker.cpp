/**
 * @file neon_number_checker.cpp
 * @brief Checks if a number is a Neon Number and explores its mathematical properties.
 *
 * Definition:
 * A Neon Number is a number where the sum of the digits of its square is equal to
 * the number itself:
 *   SumOfDigits(N^2) == N
 *
 * Known Examples in Base 10:
 * - 0: 0^2 = 0   -> Sum of digits = 0  (Neon Number)
 * - 1: 1^2 = 1   -> Sum of digits = 1  (Neon Number)
 * - 9: 9^2 = 81  -> Sum of digits = 8 + 1 = 9  (Neon Number)
 *
 * Mathematical Theorem:
 * In base 10, 0, 1, and 9 are the ONLY Neon Numbers!
 * Proof:
 * Let N have d digits (N >= 10^{d-1}).
 * Then N^2 has at most 2d digits.
 * The maximum possible sum of digits of N^2 is 9 * 2d = 18d.
 * For d >= 3, 10^{d-1} > 18d (e.g., for d = 3: 100 > 54).
 * Hence, no neon numbers can exist for N >= 100, and exhaustive testing of [0, 99]
 * confirms that only {0, 1, 9} satisfy the condition.
 *
 * Time Complexity: O(log10(N^2))
 * Space Complexity: O(1) auxiliary space
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

/**
 * @brief Computes the sum of digits of a non-negative integer.
 */
long long sumOfDigits(long long n) {
    long long sum = 0;
    while (n > 0) {
        sum += n % 10;
        n /= 10;
    }
    return sum;
}

/**
 * @brief Checks whether a given non-negative integer is a Neon Number.
 */
bool isNeon(long long n) {
    if (n < 0) return false;
    long long sq = n * n;
    return sumOfDigits(sq) == n;
}

int main() {
    std::cout << "=== Neon Number Checker & Mathematical Analysis ===\n\n";

    // 1. Evaluate specific numbers
    std::vector<long long> testNumbers = {0, 1, 4, 9, 12, 18, 45, 81, 99};

    std::cout << "1. Verification of Selected Numbers:\n";
    std::cout << "  " << std::setw(8) << "Number"
              << std::setw(14) << "Square (N^2)"
              << std::setw(18) << "SumOfDigits(N^2)"
              << std::setw(16) << "Is Neon Number?" << '\n';
    std::cout << "  " << std::string(58, '-') << '\n';

    for (long long num : testNumbers) {
        long long sq = num * num;
        long long sumSq = sumOfDigits(sq);
        bool neon = isNeon(num);

        std::cout << "  " << std::setw(8) << num
                  << std::setw(14) << sq
                  << std::setw(18) << sumSq
                  << std::setw(16) << (neon ? "YES" : "No") << '\n';
    }

    // 2. Exhaustive search across [0, 100000]
    std::cout << "\n2. Exhaustive Search for all Neon Numbers in [0, 100,000]:\n  Found: ";
    std::vector<long long> allNeon;
    for (long long i = 0; i <= 100000; ++i) {
        if (isNeon(i)) {
            allNeon.push_back(i);
            std::cout << i << " ";
        }
    }
    std::cout << "\n\nTotal found: " << allNeon.size() << " (Proving that {0, 1, 9} are the only base-10 Neon numbers)\n";

    return 0;
}
