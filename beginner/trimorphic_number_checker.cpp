/**
 * @file trimorphic_number_checker.cpp
 * @brief Checks if a number is a Trimorphic (spherical) number and finds trimorphic numbers in a range.
 *
 * Definition:
 * A Trimorphic Number (also called a spherical number) is an integer whose cube ends
 * in the same digits as the number itself:
 *   N^3 % 10^d == N   (where d is the number of digits in N)
 *
 * Comparison with Automorphic Numbers:
 * - Automorphic: N^2 ends with N (e.g., 5^2 = 25, 6^2 = 36, 76^2 = 5776).
 * - Trimorphic:  N^3 ends with N (e.g., 4^3 = 64, 9^3 = 729, 24^3 = 13824).
 * Every automorphic number is trimorphic, but not all trimorphic numbers are automorphic!
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

/**
 * @brief Checks whether a given non-negative integer is a Trimorphic Number.
 */
bool isTrimorphic(long long n) {
    if (n < 0) return false;
    if (n == 0 || n == 1) return true;

    // Use 128-bit integer to prevent overflow during cubing
    __int128 cube = static_cast<__int128>(n) * n * n;
    long long temp = n;
    __int128 modulus = 1;

    while (temp > 0) {
        modulus *= 10;
        temp /= 10;
    }

    return (cube % modulus) == n;
}

/**
 * @brief Generates all Trimorphic numbers up to a specified upper bound.
 */
std::vector<long long> findTrimorphicNumbers(long long limit) {
    std::vector<long long> results;
    for (long long i = 0; i <= limit; ++i) {
        if (isTrimorphic(i)) {
            results.push_back(i);
        }
    }
    return results;
}

int main() {
    std::cout << "=== Trimorphic (Spherical) Number Checker & Generator ===\n\n";

    // 1. Evaluate selected numbers
    std::vector<long long> testCandidates = {0, 1, 4, 5, 6, 7, 9, 24, 25, 49, 51, 75, 76, 99};

    std::cout << "1. Verification of Selected Numbers:\n";
    std::cout << "  " << std::setw(8) << "Number"
              << std::setw(16) << "Cube (N^3)"
              << std::setw(18) << "Is Trimorphic?" << '\n';
    std::cout << "  " << std::string(44, '-') << '\n';

    for (long long num : testCandidates) {
        long long cube = num * num * num;
        bool trimorphic = isTrimorphic(num);
        std::cout << "  " << std::setw(8) << num
                  << std::setw(16) << cube
                  << std::setw(18) << (trimorphic ? "YES" : "No") << '\n';
    }

    // 2. Generate all Trimorphic numbers up to 1,000
    std::cout << "\n2. All Trimorphic Numbers up to 1,000:\n  ";
    auto list = findTrimorphicNumbers(1000);
    for (size_t i = 0; i < list.size(); ++i) {
        std::cout << list[i];
        if (i + 1 < list.size()) std::cout << ", ";
    }
    std::cout << "\n\nTotal found in [0, 1000]: " << list.size() << '\n';

    return 0;
}
