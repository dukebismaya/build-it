/*
 * Program Description:
 * Miller-Rabin Deterministic Primality Test for 64-bit Integers.
 * An efficient primality testing algorithm running in O(k * log^3 N) time.
 *
 * Deterministic Basis:
 * For numbers up to 2^64, testing against the set of the first 12 primes:
 * {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}
 * is mathematically proven to be 100% deterministic (no false positives/Carmichael numbers).
 *
 * Algorithm Concept:
 * Write N - 1 as 2^s * d where d is odd.
 * For each witness base a:
 * - Compute x = a^d mod N.
 * - If x == 1 or x == N - 1, base a passes.
 * - Repeatedly square x up to s - 1 times. If x == N - 1, base a passes.
 * - If x never reaches N - 1, N is composite.
 */

#include <iostream>
#include <vector>

using u64 = unsigned long long;
using u128 = __int128_t;

// Modular multiplication (a * b) % mod to avoid 64-bit overflow
u64 modMul(u64 a, u64 b, u64 mod) {
    return static_cast<u64>((static_cast<u128>(a) * b) % mod);
}

// Modular exponentiation (base^exp) % mod
u64 modPow(u64 base, u64 exp, u64 mod) {
    u64 res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) res = modMul(res, base, mod);
        base = modMul(base, base, mod);
        exp >>= 1;
    }
    return res;
}

// Miller-Rabin test for a single witness base a
bool checkComposite(u64 n, u64 a, u64 d, int s) {
    u64 x = modPow(a, d, n);
    if (x == 1 || x == n - 1) return false; // Probably prime for this base

    for (int r = 1; r < s; ++r) {
        x = modMul(x, x, n);
        if (x == n - 1) return false;
    }
    return true; // Definitely composite
}

// Deterministic Miller-Rabin primality test for 64-bit integers
bool isPrimeMillerRabin(u64 n) {
    if (n < 2) return false;
    if (n == 2 || n == 3 || n == 5 || n == 7) return true;
    if (n % 2 == 0 || n % 3 == 0 || n % 5 == 0) return false;

    // Write n - 1 as d * 2^s
    u64 d = n - 1;
    int s = 0;
    while ((d & 1) == 0) {
        d >>= 1;
        s++;
    }

    // Deterministic bases for all 64-bit numbers (proven by Pomerance et al.)
    static const u64 bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};

    for (u64 a : bases) {
        if (n <= a) break;
        if (checkComposite(n, a, d, s)) {
            return false;
        }
    }

    return true;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "   Miller-Rabin Primality Test (64-bit)   \n";
    std::cout << "==========================================\n\n";

    std::vector<u64> testNumbers = {
        2, 3, 4, 17, 19, 561,             // 561 is smallest Carmichael composite
        1000000007ULL,                    // Standard competitive programming prime
        1000000009ULL,                    // Standard prime
        1000000011ULL,                    // Composite (3 * 333333337)
        2147483647ULL,                    // 2^31 - 1 (Mersenne prime M31)
        2305843009213693951ULL,           // 2^61 - 1 (Mersenne prime M61)
        18446744073709551557ULL           // Large 64-bit prime
    };

    for (u64 n : testNumbers) {
        bool prime = isPrimeMillerRabin(n);
        std::cout << "Number: " << n
                  << " -> " << (prime ? "PRIME [YES]" : "COMPOSITE [NO]") << "\n";
    }

    return 0;
}
