/**
 * @file mersenne_prime_checker.cpp
 * @brief Mersenne Prime verification using the Lucas-Lehmer Test in C++17.
 *
 * A Mersenne number has the form M_p = 2^p - 1. If M_p is prime, it is called
 * a Mersenne prime. A necessary condition for M_p to be prime is that the exponent
 * p itself must be prime.
 *
 * The Lucas-Lehmer Primality Test:
 * For an odd prime p:
 *   Let S_0 = 4
 *   For i = 1, 2, ..., p - 2:
 *       S_i = (S_{i-1}^2 - 2) mod M_p
 *   M_p is prime if and only if S_{p-2} == 0.
 *
 * This test is deterministic and operates in O(p) iterations, each requiring
 * one multiplication and modulo by M_p. With 64-bit integers and 128-bit modular
 * multiplication, this implementation tests exponents p up to 61 (e.g., M_31, M_61).
 *
 * Complexity:
 *   - Primality test of exponent p: O(sqrt(p)).
 *   - Lucas-Lehmer test: O(p * M(b)) where b = p bits, O(1) space.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>

namespace primality {

/**
 * @brief Checks if an integer is prime using trial division.
 */
bool isSmallPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

/**
 * @brief Deterministic Lucas-Lehmer primality test for Mersenne number M_p = 2^p - 1.
 * @param p The prime exponent (p >= 2).
 * @return True if M_p is prime, false otherwise.
 */
bool lucasLehmerTest(int p) {
    if (p < 2) return false;
    if (p == 2) return true; // M_2 = 2^2 - 1 = 3 is prime
    if (!isSmallPrime(p)) return false;

    // For p >= 64, uint64_t overflow would occur on M_p.
    if (p >= 63) {
        std::cerr << "Warning: Exponent p=" << p << " exceeds 63-bit range for this demonstration.\n";
        return false;
    }

    uint64_t mp = (1ULL << p) - 1ULL;
    unsigned __int128 s = 4;

    for (int i = 0; i < p - 2; ++i) {
        s = (s * s - 2) % mp;
    }

    return (s == 0);
}

} // namespace primality

int main() {
    std::cout << "========================================\n";
    std::cout << "      MERSENNE PRIME CHECKER            \n";
    std::cout << "     (Lucas-Lehmer Primality Test)      \n";
    std::cout << "========================================\n\n";

    std::vector<int> candidateExponents = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31};

    std::cout << std::left << std::setw(6)  << "p"
              << std::setw(15) << "M_p = 2^p - 1"
              << std::setw(18) << "Exponent Prime?"
              << "Is Mersenne Prime?\n";
    std::cout << std::string(60, '-') << "\n";

    for (int p : candidateExponents) {
        uint64_t mp = (1ULL << p) - 1ULL;
        bool expPrime = primality::isSmallPrime(p);
        bool isMersenne = primality::lucasLehmerTest(p);

        std::cout << std::left << std::setw(6) << p
                  << std::setw(15) << mp
                  << std::setw(18) << (expPrime ? "Yes" : "No")
                  << (isMersenne ? "PRIME" : "Composite") << "\n";
    }

    std::cout << "\nNotes:\n";
    std::cout << "  - p = 11: M_11 = 2047 = 23 * 89 (Composite, despite 11 being prime)\n";
    std::cout << "  - p = 23: M_23 = 8388607 = 47 * 178481 (Composite)\n";
    std::cout << "  - p = 29: M_29 = 536870911 = 233 * 1103 * 2089 (Composite)\n";
    std::cout << "  - p = 31: M_31 = 2147483647 is the celebrated 8th Mersenne prime (Euler, 1772)\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
