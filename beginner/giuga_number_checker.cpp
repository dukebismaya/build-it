/**
 * @file giuga_number_checker.cpp
 * @brief Analysis and verification of Giuga Numbers in C++17.
 *
 * Mathematical Definition:
 *   A Giuga number (named after Giuseppe Giuga) is a composite positive integer n
 *   such that for every prime factor p of n:
 *     p divides (n / p - 1)
 *
 * Equivalent Characterizations:
 *   1. For every prime factor p of n: n / p == 1 (mod p).
 *   2. For every prime factor p of n: n == p (mod p^2), i.e., p^2 divides (n - p).
 *   3. Sum of reciprocals property:
 *        sum_{p | n} (1 / p) - prod_{p | n} (1 / p)  is an integer.
 *
 * Known Giuga Numbers:
 *   Giuga numbers are extraordinarily rare. The first four are:
 *     - 30    = 2 * 3 * 5
 *     - 858   = 2 * 3 * 11 * 13
 *     - 1722  = 2 * 3 * 7 * 41
 *     - 66198 = 2 * 3 * 11 * 17 * 31
 *
 * Agoh-Giuga Conjecture (1950):
 *   An integer n >= 2 is prime if and only if sum_{i=1}^{n-1} i^(n-1) == -1 (mod n).
 *   Any counterexample to this conjecture must be a Giuga number with more than
 *   13,800 digits!
 *
 * Complexity:
 *   - Prime factorization: O(sqrt(n)).
 *   - Giuga condition check: O(number of prime factors) = O(log n).
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <numeric>

namespace number_theory {

struct PrimeFactorInfo {
    uint64_t p;
    uint64_t quotient;    // n / p
    uint64_t quotientMinusOne; // (n / p - 1)
    bool conditionSatisfied; // p | (n / p - 1)
};

struct GiugaResult {
    uint64_t n;
    bool isComposite;
    bool isGiuga;
    std::vector<PrimeFactorInfo> factors;
};

/**
 * @brief Checks if n is a Giuga number by inspecting each prime factor p.
 */
GiugaResult checkGiuga(uint64_t n) {
    if (n < 4) {
        return {n, false, false, {}};
    }

    uint64_t temp = n;
    std::vector<uint64_t> primes;

    for (uint64_t d = 2; d * d <= temp; ++d) {
        if (temp % d == 0) {
            primes.push_back(d);
            while (temp % d == 0) {
                temp /= d;
            }
        }
    }
    if (temp > 1) {
        primes.push_back(temp);
    }

    // Must be composite (at least 2 distinct prime factors)
    if (primes.size() <= 1 && primes[0] == n) {
        return {n, false, false, {}}; // n is prime
    }

    std::vector<PrimeFactorInfo> factorDetails;
    bool allSatisfied = true;

    for (uint64_t p : primes) {
        uint64_t q = n / p;
        uint64_t qMinus1 = q - 1;
        bool ok = (qMinus1 % p == 0);
        if (!ok) allSatisfied = false;

        factorDetails.push_back({p, q, qMinus1, ok});
    }

    return {n, true, allSatisfied, factorDetails};
}

} // namespace number_theory

int main() {
    std::cout << "========================================\n";
    std::cout << "         GIUGA NUMBER CHECKER           \n";
    std::cout << "    (p | (n/p - 1) for all p | n)       \n";
    std::cout << "========================================\n\n";

    std::vector<uint64_t> testNumbers = {30, 858, 1722, 60, 100, 561};

    for (uint64_t n : testNumbers) {
        auto res = number_theory::checkGiuga(n);

        std::cout << "Evaluating n = " << n << ":\n";
        if (!res.isComposite) {
            std::cout << "  n is prime (Giuga numbers must be composite).\n\n";
            continue;
        }

        std::cout << "  Prime Factor Verification (Checking if p | (n/p - 1)):\n";
        for (const auto& f : res.factors) {
            std::cout << "    - Prime p = " << std::setw(3) << f.p
                      << ": n/p = " << std::setw(5) << f.quotient
                      << ", (n/p - 1) = " << std::setw(5) << f.quotientMinusOne
                      << " -> " << f.quotientMinusOne << " mod " << f.p << " = "
                      << (f.quotientMinusOne % f.p)
                      << (f.conditionSatisfied ? " [SATISFIED]" : " [FAILS]") << "\n";
        }

        std::cout << "  Conclusion: "
                  << (res.isGiuga ? "YES -> GIUGA NUMBER!" : "NO (Condition fails for at least one factor)")
                  << "\n\n";
    }

    std::cout << "Program finished successfully.\n";
    return 0;
}
