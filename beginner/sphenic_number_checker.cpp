/**
 * @file sphenic_number_checker.cpp
 * @brief Checks if a number is a Sphenic Number and analyzes its divisor structure.
 *
 * Definition:
 * A Sphenic Number is a positive integer that is the product of exactly THREE DISTINCT
 * prime numbers:
 *   N = p * q * r   (where p < q < r are distinct primes)
 *
 * Properties of Sphenic Numbers:
 * 1. N is strictly square-free (no prime factor has exponent > 1).
 * 2. Every Sphenic number has exactly 8 positive divisors:
 *      {1, p, q, r, p*q, p*r, q*r, p*q*r}
 * 3. The Mobius function mu(N) = (-1)^3 = -1.
 *
 * Examples:
 * - 30:  2 * 3 * 5   (Divisors: 1, 2, 3, 5, 6, 10, 15, 30 -> 8 divisors)
 * - 42:  2 * 3 * 7
 * - 66:  2 * 3 * 11
 * - 70:  2 * 5 * 7
 * - 105: 3 * 5 * 7
 * - 60:  2^2 * 3 * 5 (Not square-free -> Not Sphenic)
 *
 * Time Complexity: O(sqrt(N)) for trial division
 * Space Complexity: O(1) auxiliary space
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

struct SphenicResult {
    bool isSphenic;
    std::vector<long long> distinctPrimes;
    std::vector<long long> allDivisors;
};

/**
 * @brief Checks if a number is Sphenic and factors it.
 */
SphenicResult checkSphenic(long long n) {
    if (n <= 0) return {false, {}, {}};

    long long temp = n;
    std::vector<long long> primeFactors;

    for (long long d = 2; d * d <= temp; ++d) {
        if (temp % d == 0) {
            primeFactors.push_back(d);
            temp /= d;
            if (temp % d == 0) {
                // Not square-free
                return {false, {}, {}};
            }
        }
    }

    if (temp > 1) {
        primeFactors.push_back(temp);
    }

    if (primeFactors.size() != 3) {
        return {false, primeFactors, {}};
    }

    // Find all 8 divisors
    std::vector<long long> divisors;
    long long p = primeFactors[0];
    long long q = primeFactors[1];
    long long r = primeFactors[2];
    divisors = {1, p, q, r, p * q, p * r, q * r, n};

    return {true, primeFactors, divisors};
}

int main() {
    std::cout << "=== Sphenic Number Checker & Divisor Analysis ===\n\n";

    // 1. Evaluate selected numbers
    std::vector<long long> testCandidates = {12, 30, 42, 60, 66, 70, 78, 100, 105, 110};

    std::cout << "1. Verification of Selected Numbers:\n";
    std::cout << "  " << std::setw(8) << "Number"
              << std::setw(14) << "Is Sphenic?"
              << "   Prime Factors (p * q * r)\n";
    std::cout << "  " << std::string(54, '-') << '\n';

    for (long long num : testCandidates) {
        auto res = checkSphenic(num);
        std::cout << "  " << std::setw(8) << num
                  << std::setw(14) << (res.isSphenic ? "YES" : "No") << "   ";

        if (res.isSphenic) {
            std::cout << res.distinctPrimes[0] << " * "
                      << res.distinctPrimes[1] << " * "
                      << res.distinctPrimes[2] << " (8 divisors)\n";
        } else {
            std::cout << "-\n";
        }
    }

    // 2. Find all Sphenic numbers up to 150
    std::cout << "\n2. All Sphenic Numbers up to 150:\n  ";
    int count = 0;
    for (long long i = 1; i <= 150; ++i) {
        if (checkSphenic(i).isSphenic) {
            std::cout << i << " ";
            count++;
        }
    }
    std::cout << "\n\nTotal Sphenic numbers found in [1, 150]: " << count << '\n';

    return 0;
}

