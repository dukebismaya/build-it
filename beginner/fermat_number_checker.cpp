/**
 * @file fermat_number_checker.cpp
 * @brief Analysis and Primality Testing of Fermat Numbers in C++17.
 *
 * A Fermat number is an integer of the form:
 *   F_n = 2^(2^n) + 1,  for n >= 0.
 *
 * Historical Context:
 *   Pierre de Fermat conjectured in 1650 that all F_n are prime.
 *   The first five are prime:
 *     F_0 = 3, F_1 = 5, F_2 = 17, F_3 = 257, F_4 = 65537.
 *   In 1732, Leonhard Euler disproved Fermat's conjecture by showing that
 *   F_5 = 2^32 + 1 = 4,294,967,297 is divisible by 641:
 *     4,294,967,297 = 641 * 6,700,417.
 *
 * Pépin's Primality Test:
 *   For n >= 1, F_n is prime if and only if:
 *     3^((F_n - 1) / 2) == -1 (mod F_n), which is equivalent to:
 *     3^(2^(2^n - 1)) == F_n - 1 (mod F_n).
 *
 * Complexity:
 *   - Pépin test for F_n: (2^n - 1) squarings modulo F_n.
 *   - Overall complexity: O(2^n * M(2^n)), space O(1).
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>

namespace number_theory {

/**
 * @brief Computes (base^exp) mod mod using 128-bit unsigned arithmetic.
 */
uint64_t powerMod(uint64_t base, uint64_t exp, uint64_t mod) {
    unsigned __int128 res = 1;
    unsigned __int128 b = base % mod;
    while (exp > 0) {
        if (exp & 1) {
            res = (res * b) % mod;
        }
        b = (b * b) % mod;
        exp >>= 1;
    }
    return static_cast<uint64_t>(res);
}

/**
 * @brief Computes Fermat number F_n = 2^(2^n) + 1 for n in [0, 5].
 * @param n Index (n <= 5 fits in uint64_t).
 * @return F_n value.
 */
uint64_t computeFermatNumber(int n) {
    if (n < 0 || n > 5) return 0;
    uint64_t powerOfTwo = 1ULL << n; // 2^n
    if (powerOfTwo >= 64) {
        // F_5: 2^32 + 1
        return (1ULL << 32) + 1ULL;
    }
    return (1ULL << powerOfTwo) + 1ULL;
}

/**
 * @brief Pépin's primality test for Fermat number F_n (for n in [1, 4]).
 * @param n Index of Fermat number (1 <= n <= 4).
 * @return True if F_n is prime, false otherwise.
 */
bool pepinTest(int n) {
    if (n < 1 || n > 4) return false;
    uint64_t fn = computeFermatNumber(n);

    // Repeated squaring: 3^(2^(2^n - 1)) mod fn
    // Start with 3, square (2^n - 1) times
    uint64_t exponentSteps = (1ULL << n) - 1;
    unsigned __int128 current = 3;

    for (uint64_t step = 0; step < exponentSteps; ++step) {
        current = (current * current) % fn;
    }

    return (current == (fn - 1));
}

} // namespace number_theory

int main() {
    std::cout << "========================================\n";
    std::cout << "        FERMAT NUMBER CHECKER           \n";
    std::cout << "   (Pepin Primality Test & Factoring)   \n";
    std::cout << "========================================\n\n";

    std::cout << std::left << std::setw(6) << "n"
              << std::setw(16) << "2^n"
              << std::setw(22) << "F_n = 2^(2^n) + 1"
              << std::setw(16) << "Pepin Status"
              << "Conclusion\n";
    std::cout << std::string(75, '-') << "\n";

    // F_0 is trivially prime
    std::cout << std::left << std::setw(6) << 0
              << std::setw(16) << 1
              << std::setw(22) << 3
              << std::setw(16) << "Trivial"
              << "PRIME (Fermat Prime)\n";

    for (int n = 1; n <= 4; ++n) {
        uint64_t fn = number_theory::computeFermatNumber(n);
        uint64_t twoPow = 1ULL << n;
        bool isPrime = number_theory::pepinTest(n);

        std::cout << std::left << std::setw(6) << n
                  << std::setw(16) << twoPow
                  << std::setw(22) << fn
                  << std::setw(16) << (isPrime ? "Passed" : "Failed")
                  << (isPrime ? "PRIME (Fermat Prime)" : "COMPOSITE") << "\n";
    }

    // F_5 demonstration (Euler's counterexample)
    uint64_t f5 = number_theory::computeFermatNumber(5);
    uint64_t eulerDivisor = 641;
    uint64_t quotient = f5 / eulerDivisor;
    uint64_t remainder = f5 % eulerDivisor;

    std::cout << std::left << std::setw(6) << 5
              << std::setw(16) << 32
              << std::setw(22) << f5
              << std::setw(16) << "Factored"
              << "COMPOSITE (Euler, 1732)\n";

    std::cout << "\nEuler Factorization Details for F_5:\n";
    std::cout << "  F_5 = " << f5 << "\n";
    std::cout << "  F_5 mod " << eulerDivisor << " = " << remainder << "\n";
    std::cout << "  " << f5 << " = " << eulerDivisor << " * " << quotient << "\n";
    std::cout << "  Both factors (641 and " << quotient << ") are prime.\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
