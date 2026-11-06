/**
 * @file magic_number_checker.cpp
 * @brief Checks if a number is a Magic Number (digital root equals 1) and explores modular arithmetic.
 *
 * Definition:
 * A Magic Number is a positive integer in which the recursive sum of its decimal digits
 * (its digital root) eventually terminates at 1.
 *
 * Examples:
 * - 19:   1 + 9 = 10        -> 1 + 0 = 1        (Magic Number)
 * - 28:   2 + 8 = 10        -> 1 + 0 = 1        (Magic Number)
 * - 55:   5 + 5 = 10        -> 1 + 0 = 1        (Magic Number)
 * - 1234: 1 + 2 + 3 + 4 = 10 -> 1 + 0 = 1       (Magic Number)
 * - 226:  2 + 2 + 6 = 10    -> 1 + 0 = 1        (Magic Number)
 * - 1000: 1 + 0 + 0 + 0 = 1                     (Magic Number)
 *
 * Mathematical Theorem:
 * The digital root of any positive integer N is given by:
 *   DigitalRoot(N) = 1 + ((N - 1) % 9)
 * Therefore:
 *   N is a Magic Number  <=>  N % 9 == 1
 *
 * Time Complexity:
 * - Iterative reduction: O(log10(N))
 * - Direct modular check: O(1)
 * Space Complexity: O(1)
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

struct MagicAnalysis {
    bool isMagic;
    long long digitalRoot;
    std::vector<long long> reductionSteps;
};

/**
 * @brief Computes the sum of decimal digits of n.
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
 * @brief Performs step-by-step reduction of n to its digital root.
 */
MagicAnalysis analyzeMagicNumber(long long n) {
    if (n <= 0) return {false, 0, {}};

    std::vector<long long> steps = {n};
    long long curr = n;

    while (curr >= 10) {
        curr = sumOfDigits(curr);
        steps.push_back(curr);
    }

    return {curr == 1, curr, steps};
}

int main() {
    std::cout << "=== Magic Number Checker & Digital Root Analysis ===\n\n";

    // 1. Evaluate selected candidates
    std::vector<long long> testCandidates = {19, 28, 35, 55, 100, 226, 1234, 1729, 2026};

    std::cout << "1. Verification of Selected Numbers:\n";
    std::cout << "  " << std::setw(8) << "Number"
              << std::setw(14) << "Digital Root"
              << std::setw(14) << "Is Magic?"
              << "   Reduction Sequence\n";
    std::cout << "  " << std::string(62, '-') << '\n';

    for (long long num : testCandidates) {
        auto res = analyzeMagicNumber(num);
        std::cout << "  " << std::setw(8) << num
                  << std::setw(14) << res.digitalRoot
                  << std::setw(14) << (res.isMagic ? "YES" : "No") << "   ";

        for (size_t i = 0; i < res.reductionSteps.size(); ++i) {
            std::cout << res.reductionSteps[i];
            if (i + 1 < res.reductionSteps.size()) std::cout << " -> ";
        }
        std::cout << '\n';
    }

    // 2. Range search up to 100
    std::cout << "\n2. All Magic Numbers in [1, 100] (matching N % 9 == 1):\n  ";
    int count = 0;
    for (long long i = 1; i <= 100; ++i) {
        if (analyzeMagicNumber(i).isMagic) {
            std::cout << i << " ";
            count++;
        }
    }
    std::cout << "\n\nTotal Magic numbers found in [1, 100]: " << count << '\n';

    return 0;
}

