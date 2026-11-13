/**
 * @file thabit_number_checker.cpp
 * @brief Thabit ibn Qurra number analysis and amicable pair rule verification in C++17.
 *
 * Mathematical Definition:
 *   A Thabit number (or Thabit ibn Qurra number) is an integer of the form:
 *     T_n = 3 * 2^n - 1,  for n >= 0.
 *
 * The sequence begins:
 *   2, 5, 11, 23, 47, 95, 191, 383, 767, 1535, 3071, 6143, ...
 *
 * Thabit's Rule for Amicable Pairs (9th Century):
 *   If for an integer n > 1:
 *     p = 3 * 2^(n - 1) - 1  (i.e. T_{n-1})
 *     q = 3 * 2^n - 1        (i.e. T_n)
 *     r = 9 * 2^(2n - 1) - 1
 *   are all prime numbers, then:
 *     A = 2^n * p * q   and   B = 2^n * r
 *   form a pair of amicable numbers (each is the sum of the proper divisors of the other).
 *
 * Historic Examples:
 *   - n = 2: p = 5,  q = 11, r = 71   -> (220, 284)        (Known to Pythagoras)
 *   - n = 4: p = 23, q = 47, r = 1151 -> (17296, 18416)    (Rediscovered by Fermat, 1636)
 *   - n = 7: p = 191, q = 383, r = 73727 -> (9363584, 9437056) (Rediscovered by Descartes, 1638)
 *
 * Complexity:
 *   - Generation: O(1) per term using bit shifts.
 *   - Primality test: O(sqrt(N)) via trial division.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <optional>

namespace number_theory {

/**
 * @brief Computes the nth Thabit number T_n = 3 * 2^n - 1.
 */
uint64_t computeThabit(int n) {
    if (n < 0 || n > 60) return 0;
    return 3ULL * (1ULL << n) - 1ULL;
}

/**
 * @brief Trial division primality check.
 */
bool isPrime(uint64_t n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (uint64_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

struct AmicablePair {
    int n;
    uint64_t p;
    uint64_t q;
    uint64_t r;
    uint64_t a;
    uint64_t b;
};

/**
 * @brief Tests Thabit's amicable pair construction for a given exponent n.
 */
std::optional<AmicablePair> checkThabitAmicableRule(int n) {
    if (n < 2 || n > 25) return std::nullopt;

    uint64_t p = 3ULL * (1ULL << (n - 1)) - 1ULL;
    uint64_t q = 3ULL * (1ULL << n) - 1ULL;
    uint64_t r = 9ULL * (1ULL << (2 * n - 1)) - 1ULL;

    if (isPrime(p) && isPrime(q) && isPrime(r)) {
        uint64_t a = (1ULL << n) * p * q;
        uint64_t b = (1ULL << n) * r;
        return AmicablePair{n, p, q, r, a, b};
    }
    return std::nullopt;
}

} // namespace number_theory

int main() {
    std::cout << "========================================\n";
    std::cout << "        THABIT NUMBER CHECKER           \n";
    std::cout << "    (Thabit Amicable Pair Theorem)      \n";
    std::cout << "========================================\n\n";

    std::cout << "First 12 Thabit Numbers T_n = 3 * 2^n - 1:\n";
    std::cout << std::left << std::setw(6)  << "n"
              << std::setw(16) << "T_n"
              << "Is Thabit Prime?\n";
    std::cout << std::string(38, '-') << "\n";

    for (int n = 0; n <= 11; ++n) {
        uint64_t t = number_theory::computeThabit(n);
        bool prime = number_theory::isPrime(t);
        std::cout << std::left << std::setw(6)  << n
                  << std::setw(16) << t
                  << (prime ? "PRIME (Thabit Prime)" : "Composite") << "\n";
    }

    std::cout << "\nThabit's Rule Amicable Pair Search (n in [2, 10]):\n";
    for (int n = 2; n <= 10; ++n) {
        auto pair = number_theory::checkThabitAmicableRule(n);
        if (pair) {
            std::cout << "  Found for n = " << pair->n << ":\n";
            std::cout << "    Primes: p = " << pair->p << ", q = " << pair->q << ", r = " << pair->r << "\n";
            std::cout << "    Amicable Pair: (" << pair->a << ", " << pair->b << ")\n";
        }
    }

    std::cout << "\nProgram finished successfully.\n";
    return 0;
}
