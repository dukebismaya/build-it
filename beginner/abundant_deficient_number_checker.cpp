/**
 * @file abundant_deficient_number_checker.cpp
 * @brief Classifies positive integers as Abundant, Deficient, or Perfect based on proper divisor sum.
 *
 * Definitions:
 * Let s(n) be the sum of all proper divisors of n (divisors strictly less than n):
 *   s(n) = sigma(n) - n
 *
 * 1. Perfect Number:   s(n) == n  (e.g., 6 -> 1 + 2 + 3 = 6; 28 -> 1 + 2 + 4 + 7 + 14 = 28)
 * 2. Deficient Number: s(n) < n   (e.g., 8 -> 1 + 2 + 4 = 7 < 8; all prime numbers)
 * 3. Abundant Number:  s(n) > n   (e.g., 12 -> 1 + 2 + 3 + 4 + 6 = 16 > 12)
 *
 * The abundance of an abundant number is defined as s(n) - n.
 *
 * Time Complexity:
 * - Single number check: O(sqrt(N))
 * - Range classification up to N: O(N * sqrt(N))
 * Space Complexity: O(sqrt(N)) to store divisors.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <string>

enum class NumberClassification {
    PERFECT,
    DEFICIENT,
    ABUNDANT
};

struct DivisorAnalysis {
    long long n;
    std::vector<long long> properDivisors;
    long long properDivisorSum;
    long long abundance; // properDivisorSum - n
    NumberClassification classification;
};

/**
 * @brief Analyzes and classifies a positive integer n.
 * @param n Positive integer.
 * @return DivisorAnalysis containing divisors, sum, abundance, and classification.
 */
DivisorAnalysis analyzeNumber(long long n) {
    if (n <= 1) {
        return {n, {}, 0, -n, NumberClassification::DEFICIENT};
    }

    std::vector<long long> divisors = {1};
    long long limit = static_cast<long long>(std::sqrt(n));

    for (long long d = 2; d <= limit; ++d) {
        if (n % d == 0) {
            divisors.push_back(d);
            if (d != n / d) {
                divisors.push_back(n / d);
            }
        }
    }

    std::sort(divisors.begin(), divisors.end());
    long long sum = std::accumulate(divisors.begin(), divisors.end(), 0LL);
    long long abundance = sum - n;

    NumberClassification classification;
    if (sum == n) {
        classification = NumberClassification::PERFECT;
    } else if (sum > n) {
        classification = NumberClassification::ABUNDANT;
    } else {
        classification = NumberClassification::DEFICIENT;
    }

    return {n, divisors, sum, abundance, classification};
}

int main() {
    std::cout << "=== Abundant, Deficient, and Perfect Number Classifier ===\n\n";

    // 1. Analyze selected integers
    std::vector<long long> testNumbers = {6, 8, 12, 15, 18, 20, 24, 28, 496, 500};

    std::cout << "1. Detailed Analysis of Selected Integers:\n";
    std::cout << "  " << std::setw(8) << "Number"
              << std::setw(14) << "Divisor Sum"
              << std::setw(14) << "Abundance"
              << std::setw(16) << "Classification"
              << "   Proper Divisors\n";
    std::cout << "  " << std::string(72, '-') << '\n';

    for (long long num : testNumbers) {
        auto analysis = analyzeNumber(num);
        std::string classStr;
        switch (analysis.classification) {
            case NumberClassification::PERFECT:   classStr = "PERFECT"; break;
            case NumberClassification::ABUNDANT:  classStr = "ABUNDANT"; break;
            case NumberClassification::DEFICIENT: classStr = "DEFICIENT"; break;
        }

        std::cout << "  " << std::setw(8) << analysis.n
                  << std::setw(14) << analysis.properDivisorSum
                  << std::setw(14) << analysis.abundance
                  << std::setw(16) << classStr << "   [";

        for (size_t i = 0; i < analysis.properDivisors.size(); ++i) {
            std::cout << analysis.properDivisors[i];
            if (i + 1 < analysis.properDivisors.size()) std::cout << ", ";
        }
        std::cout << "]\n";
    }

    // 2. Summary count in range [1, 100]
    std::cout << "\n2. Distribution of Numbers in Range [1, 100]:\n";
    int countPerfect = 0, countAbundant = 0, countDeficient = 0;
    for (int i = 1; i <= 100; ++i) {
        auto a = analyzeNumber(i);
        if (a.classification == NumberClassification::PERFECT) countPerfect++;
        else if (a.classification == NumberClassification::ABUNDANT) countAbundant++;
        else countDeficient++;
    }

    std::cout << "  Deficient Numbers: " << countDeficient << " %\n";
    std::cout << "  Abundant Numbers:  " << countAbundant << " %\n";
    std::cout << "  Perfect Numbers:   " << countPerfect << " %\n";

    return 0;
}

