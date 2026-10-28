/**
 * @file automorphic_number_checker.cpp
 * @brief Checks if a number is Automorphic (circular) and finds all automorphic numbers up to a limit.
 *
 * Definition:
 * An automorphic number (also called a circular number) is a natural number whose square
 * ends in the same digits as the number itself.
 *
 * Examples:
 * - 0^2 = 0 (ends in 0)
 * - 1^2 = 1 (ends in 1)
 * - 5^2 = 25 (ends in 5)
 * - 6^2 = 36 (ends in 6)
 * - 25^2 = 625 (ends in 25)
 * - 76^2 = 5776 (ends in 76)
 * - 376^2 = 141376 (ends in 376)
 * - 625^2 = 390625 (ends in 625)
 * - 9376^2 = 87909376 (ends in 9376)
 *
 * Time Complexity:
 * - Single number check: O(log10(N))
 * - Range search up to L: O(L * log10(L))
 * Space Complexity: O(1) auxiliary space
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

/**
 * @brief Checks if a given non-negative integer is an Automorphic number.
 * @param n The integer to check.
 * @return True if n is automorphic, false otherwise.
 */
bool isAutomorphic(long long n) {
    if (n < 0) return false;
    if (n == 0 || n == 1) return true;

    long long square = n * n;
    long long temp = n;
    long long modulus = 1;

    // Calculate 10^d where d is the number of decimal digits in n
    while (temp > 0) {
        modulus *= 10;
        temp /= 10;
    }

    return (square % modulus) == n;
}

/**
 * @brief Finds all automorphic numbers up to a specified upper bound.
 * @param limit The maximum value to check.
 * @return Vector of automorphic numbers <= limit.
 */
std::vector<long long> findAutomorphicNumbers(long long limit) {
    std::vector<long long> results;
    for (long long i = 0; i <= limit; ++i) {
        if (isAutomorphic(i)) {
            results.push_back(i);
        }
    }
    return results;
}

int main() {
    std::cout << "=== Automorphic Number Checker & Generator ===\n\n";

    // Test specific numbers
    std::vector<long long> testNumbers = {0, 1, 5, 6, 7, 25, 76, 100, 376, 625, 9376, 12345};

    std::cout << "1. Individual Number Evaluation:\n";
    std::cout << "  " << std::setw(10) << "Number"
              << std::setw(18) << "Square"
              << std::setw(16) << "Is Automorphic?" << '\n';
    std::cout << "  " << std::string(44, '-') << '\n';

    for (long long n : testNumbers) {
        long long sq = n * n;
        bool autoNum = isAutomorphic(n);
        std::cout << "  " << std::setw(10) << n
                  << std::setw(18) << sq
                  << std::setw(16) << (autoNum ? "YES" : "No") << '\n';
    }

    std::cout << "\n2. Finding all Automorphic numbers up to 10,000:\n  ";
    auto found = findAutomorphicNumbers(10000);
    for (size_t i = 0; i < found.size(); ++i) {
        std::cout << found[i];
        if (i + 1 < found.size()) std::cout << ", ";
    }
    std::cout << "\n\nTotal found: " << found.size() << '\n';

    return 0;
}
