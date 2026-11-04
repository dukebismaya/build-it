/**
 * @file smith_number_checker.cpp
 * @brief Checks if a number is a Smith Number and finds Smith numbers in a given range.
 *
 * Definition:
 * A Smith Number is a composite number for which the sum of its decimal digits
 * is equal to the sum of the digits of all its prime factors (counted with multiplicity).
 * Prime numbers are excluded by definition.
 *
 * Examples:
 * - 4:   4 = 2 * 2
 *        Digit sum = 4, Prime factors digit sum = 2 + 2 = 4 (Smith Number)
 * - 22:  22 = 2 * 11
 *        Digit sum = 2 + 2 = 4, Prime factors digit sum = 2 + (1 + 1) = 4 (Smith Number)
 * - 27:  27 = 3 * 3 * 3
 *        Digit sum = 2 + 7 = 9, Prime factors digit sum = 3 + 3 + 3 = 9 (Smith Number)
 * - 58:  58 = 2 * 29
 *        Digit sum = 5 + 8 = 13, Prime factors digit sum = 2 + (2 + 9) = 13 (Smith Number)
 * - 666: 666 = 2 * 3 * 3 * 37
 *        Digit sum = 6 + 6 + 6 = 18, Prime factors digit sum = 2 + 3 + 3 + (3 + 7) = 18 (Smith Number)
 *
 * Time Complexity:
 * - Single check: O(sqrt(N))
 * - Range search up to N: O(N * sqrt(N))
 * Space Complexity: O(log N) for prime factor list
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <string>

/**
 * @brief Computes the sum of decimal digits of a non-negative integer.
 */
long long sumOfDigits(long long n) {
    long long sum = 0;
    while (n > 0) {
        sum += n % 10;
        n /= 10;
    }
    return sum;
}

struct SmithAnalysis {
    bool isSmith;
    bool isComposite;
    long long digitSum;
    long long factorDigitSum;
    std::vector<long long> primeFactors;
};

/**
 * @brief Checks if a number is a Smith number and performs full prime factorization.
 */
SmithAnalysis analyzeSmith(long long n) {
    if (n <= 3) {
        return {false, false, sumOfDigits(n), 0, {}};
    }

    long long digitSum = sumOfDigits(n);
    std::vector<long long> factors;
    long long factorDigitSum = 0;

    // Trial division for factor 2
    while (n % 2 == 0) {
        factors.push_back(2);
        factorDigitSum += sumOfDigits(2);
        n /= 2;
    }

    // Trial division for odd numbers up to sqrt(n)
    for (long long d = 3; d * d <= n; d += 2) {
        while (n % d == 0) {
            factors.push_back(d);
            factorDigitSum += sumOfDigits(d);
            n /= d;
        }
    }

    if (n > 1) {
        factors.push_back(n);
        factorDigitSum += sumOfDigits(n);
    }

    // Must be composite: at least 2 prime factors (or one prime factor with exponent >= 2)
    bool isComposite = (factors.size() >= 2);
    bool isSmith = isComposite && (digitSum == factorDigitSum);

    return {isSmith, isComposite, digitSum, factorDigitSum, factors};
}

int main() {
    std::cout << "=== Smith Number Checker & Factorization Analysis ===\n\n";

    // 1. Evaluate selected numbers
    std::vector<long long> testCandidates = {4, 7, 22, 27, 58, 85, 94, 121, 666};

    std::cout << "1. Detailed Verification of Selected Numbers:\n";
    std::cout << "  " << std::setw(8) << "Number"
              << std::setw(12) << "Digit Sum"
              << std::setw(16) << "Factors Dig Sum"
              << std::setw(14) << "Is Smith?"
              << "   Prime Factorization\n";
    std::cout << "  " << std::string(72, '-') << '\n';

    for (long long num : testCandidates) {
        auto res = analyzeSmith(num);
        std::cout << "  " << std::setw(8) << num
                  << std::setw(12) << res.digitSum
                  << std::setw(16) << res.factorDigitSum
                  << std::setw(14) << (res.isSmith ? "YES" : "No") << "   ";

        for (size_t i = 0; i < res.primeFactors.size(); ++i) {
            std::cout << res.primeFactors[i];
            if (i + 1 < res.primeFactors.size()) std::cout << " * ";
        }
        std::cout << '\n';
    }

    // 2. Find all Smith numbers up to 200
    std::cout << "\n2. All Smith Numbers up to 200:\n  ";
    int count = 0;
    for (long long i = 1; i <= 200; ++i) {
        if (analyzeSmith(i).isSmith) {
            std::cout << i << " ";
            count++;
        }
    }
    std::cout << "\n\nTotal Smith numbers found in [1, 200]: " << count << '\n';

    return 0;
}
