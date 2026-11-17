/**
 * ============================================================================
 * Lucas-Lehmer Primality Test for Mersenne Numbers (M_p = 2^p - 1)
 * ============================================================================
 * 
 * Description:
 * The Lucas-Lehmer test (devised by Edouard Lucas in 1878 and improved by
 * Derrick Henry Lehmer in 1930) is a deterministic primality test for
 * Mersenne numbers:
 *   M_p = 2^p - 1  (where p is an odd prime).
 * 
 * Mathematical Recurrence:
 * Define the sequence {S_i}:
 *   S_0 = 4
 *   S_i = (S_{i-1}^2 - 2) mod M_p   for i >= 1.
 * 
 * Theorem (Lucas-Lehmer):
 * For any odd prime p, the Mersenne number M_p is prime IF AND ONLY IF:
 *   S_{p - 2} = 0 (mod M_p).
 * 
 * Fast Mersenne Modulo Arithmetic:
 * Since M_p = 2^p - 1, we have 2^p = 1 (mod M_p).
 * Therefore, for any large integer X:
 *   X = A * 2^p + B = A + B (mod M_p)
 * where B = X & M_p and A = X >> p.
 * This bitwise trick computes modular reductions in O(1) shifts and additions
 * without expensive division instructions!
 * 
 * Known Mersenne Primes Tested:
 * - p=2: M_2 = 3 (prime)
 * - p=3: M_3 = 7 (prime)
 * - p=5: M_5 = 31 (prime)
 * - p=7: M_7 = 127 (prime)
 * - p=11: M_11 = 2047 = 23 * 89 (composite!)
 * - p=13: M_13 = 8191 (prime)
 * - p=17: M_17 = 131071 (prime)
 * - p=19: M_19 = 524287 (prime)
 * - p=31: M_31 = 2147483647 (prime)
 * 
 * Time Complexity:
 * - O(p) iterations, each taking O(p^2) bit operations.
 * - Total: O(p^3) or O(p * log p) with FFT.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

namespace lucas_lehmer {

/**
 * Checks if a small integer p is prime.
 */
bool isSmallPrime(int p) {
    if (p < 2) return false;
    for (int d = 2; d * d <= p; ++d) {
        if (p % d == 0) return false;
    }
    return true;
}

/**
 * Executes the Lucas-Lehmer test for M_p = 2^p - 1 using 128-bit arithmetic.
 * Valid for p <= 61 (where M_p fits in uint64_t and S_i fits in __int128).
 */
bool testMersennePrime(int p) {
    if (p == 2) return true; // M_2 = 3 is prime
    if (!isSmallPrime(p)) return false; // p must be prime

    uint64_t mp = (1ULL << p) - 1ULL;
    __int128 s = 4;

    for (int i = 0; i < p - 2; ++i) {
        __int128 sq = s * s - 2;

        // Fast Mersenne reduction: X mod (2^p - 1) = (X & M_p) + (X >> p)
        __int128 reduced = (sq & mp) + (sq >> p);
        if (reduced >= mp) {
            reduced -= mp;
        }
        s = reduced;
    }

    return s == 0;
}

} // namespace lucas_lehmer

int main() {
    std::cout << "========================================================\n";
    std::cout << " Lucas-Lehmer Primality Test for Mersenne Numbers\n";
    std::cout << "========================================================\n\n";

    int test_exponents[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31};

    std::cout << "Testing Mersenne numbers M_p = 2^p - 1 for prime p:\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << std::setw(6) << "p"
              << std::setw(16) << "M_p = 2^p - 1"
              << std::setw(18) << "Lucas-Lehmer"
              << std::setw(14) << "Verdict\n";
    std::cout << "--------------------------------------------------------\n";

    for (int p : test_exponents) {
        uint64_t mp = (1ULL << p) - 1ULL;
        bool is_prime = lucas_lehmer::testMersennePrime(p);

        std::cout << std::setw(6) << p
                  << std::setw(16) << mp
                  << std::setw(18) << (is_prime ? "S_{p-2} == 0" : "S_{p-2} != 0")
                  << std::setw(14) << (is_prime ? "PRIME" : "COMPOSITE")
                  << "\n";
    }

    std::cout << "--------------------------------------------------------\n\n";

    std::cout << "Highlight on M_11:\n";
    std::cout << "  p = 11: 2^11 - 1 = 2047 = 23 * 89 -> Correctly identified as COMPOSITE!\n";
    std::cout << "Highlight on M_31:\n";
    std::cout << "  p = 31: 2^31 - 1 = 2,147,483,647 -> Famously proved prime by Euler in 1772!\n";

    return 0;
}
