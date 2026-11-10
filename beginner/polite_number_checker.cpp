/**
 * @file polite_number_checker.cpp
 * @brief Analysis, politeness calculation, and partition generation for Polite Numbers in C++17.
 *
 * Definition:
 *   A polite number is a positive integer that can be expressed as the sum of
 *   two or more consecutive positive integers.
 *   For example:
 *     9 = 4 + 5 = 2 + 3 + 4
 *     15 = 7 + 8 = 4 + 5 + 6 = 1 + 2 + 3 + 4 + 5
 *
 * Politeness Properties:
 *   - A positive integer is polite if and only if it is NOT a power of 2 (i.e. N != 2^k).
 *     Powers of 2 (1, 2, 4, 8, 16, 32, ...) are the only "impolite" numbers.
 *   - The "politeness" of N is the number of ways N can be written as the sum of
 *     consecutive positive integers.
 *   - Politeness Theorem: The politeness of N equals the number of odd divisors of N
 *     greater than 1. If N = 2^a * p_1^e_1 * p_2^e_2 * ... where p_i are odd primes:
 *       Politeness(N) = (e_1 + 1) * (e_2 + 1) * ... - 1.
 *
 * Sum representation:
 *   If N = sum_{i=0}^{k-1} (a + i) = k * a + k * (k - 1) / 2, then:
 *     k * a = N - k * (k - 1) / 2 > 0
 *   Thus, for each length k >= 2 such that (N - k * (k - 1) / 2) is divisible by k,
 *   a valid starting integer 'a' exists.
 *
 * Complexity:
 *   - Politeness calculation via prime factorization: O(sqrt(N)) time.
 *   - Consecutive partition generation: O(sqrt(N)) time, O(politeness) space.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <string>

namespace number_theory {

struct ConsecutivePartition {
    int64_t start;
    int64_t length;
};

/**
 * @brief Checks if a number is a power of 2 (impolite).
 */
bool isPowerOfTwo(uint64_t n) {
    return n > 0 && (n & (n - 1)) == 0;
}

/**
 * @brief Checks if a number is polite (has at least one consecutive sum representation).
 */
bool isPolite(uint64_t n) {
    return n > 1 && !isPowerOfTwo(n);
}

/**
 * @brief Computes the politeness of N (number of odd divisors > 1).
 */
uint64_t computePoliteness(uint64_t n) {
    if (n <= 1) return 0;

    // Remove factors of 2
    while (n % 2 == 0) {
        n /= 2;
    }

    uint64_t oddDivisorsCount = 1;
    for (uint64_t d = 3; d * d <= n; d += 2) {
        if (n % d == 0) {
            uint64_t exponent = 0;
            while (n % d == 0) {
                exponent++;
                n /= d;
            }
            oddDivisorsCount *= (exponent + 1);
        }
    }
    if (n > 1) {
        oddDivisorsCount *= 2; // n is prime
    }

    return oddDivisorsCount - 1; // Exclude the odd divisor 1
}

/**
 * @brief Generates all consecutive integer partitions of N.
 */
std::vector<ConsecutivePartition> getConsecutivePartitions(int64_t n) {
    std::vector<ConsecutivePartition> partitions;
    if (n <= 2) return partitions;

    // k is the number of terms; k * (k - 1) / 2 < n
    for (int64_t k = 2; k * (k - 1) / 2 < n; ++k) {
        int64_t rem = n - k * (k - 1) / 2;
        if (rem % k == 0) {
            int64_t a = rem / k;
            if (a > 0) {
                partitions.push_back({a, k});
            }
        }
    }
    return partitions;
}

} // namespace number_theory

int main() {
    std::cout << "========================================\n";
    std::cout << "         POLITE NUMBER CHECKER          \n";
    std::cout << "========================================\n\n";

    std::vector<uint64_t> testNumbers = {1, 2, 3, 4, 9, 15, 16, 21, 30, 32, 45, 64};

    std::cout << std::left << std::setw(6)  << "N"
              << std::setw(14) << "Is Polite?"
              << std::setw(14) << "Politeness"
              << "Consecutive Integer Partitions\n";
    std::cout << std::string(75, '-') << "\n";

    for (uint64_t num : testNumbers) {
        bool polite = number_theory::isPolite(num);
        uint64_t count = number_theory::computePoliteness(num);
        auto partitions = number_theory::getConsecutivePartitions(num);

        std::string partStr;
        if (partitions.empty()) {
            partStr = "(None - Power of 2)";
        } else {
            for (size_t i = 0; i < partitions.size(); ++i) {
                partStr += "[";
                for (int64_t j = 0; j < partitions[i].length; ++j) {
                    partStr += std::to_string(partitions[i].start + j);
                    if (j + 1 < partitions[i].length) partStr += "+";
                }
                partStr += "]";
                if (i + 1 < partitions.size()) partStr += ", ";
            }
        }

        std::cout << std::left << std::setw(6)  << num
                  << std::setw(14) << (polite ? "YES" : "NO (Impolite)")
                  << std::setw(14) << count
                  << partStr << "\n";
    }

    std::cout << "\nObservations:\n";
    std::cout << "  - Numbers 1, 2, 4, 16, 32, 64 cannot be written as sum of consecutive ints.\n";
    std::cout << "  - 15 has politeness 3: [7+8], [4+5+6], [1+2+3+4+5].\n";
    std::cout << "  - Politeness matches the number of odd divisors strictly greater than 1.\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
