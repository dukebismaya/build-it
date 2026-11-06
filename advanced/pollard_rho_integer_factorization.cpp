/**
 * @file pollard_rho_integer_factorization.cpp
 * @brief Factors large 64-bit integers in O(N^1/4) using Pollard's Rho with Brent's cycle detection.
 *
 * Theory:
 * Invented by John Pollard in 1975 and optimized by Richard Brent in 1980.
 * By the Birthday Paradox, the sequence x_{k+1} = (x_k^2 + c) mod N will cycle modulo a
 * prime factor p | N in approximately O(sqrt(p)) <= O(N^1/4) steps.
 *
 * Key Optimizations:
 * 1. Brent's Cycle Finding:
 *    Instead of Floyd's Tortoise and Hare, Brent compares x_k with x_{2^m}, doubling the power of 2.
 * 2. Product Batching:
 *    Instead of computing gcd(|x - y|, N) at every step, accumulate products of |x - y| modulo N
 *    and compute a single gcd every 128 steps.
 * 3. 64-bit Miller-Rabin Primality Test:
 *    Uses deterministic witnesses {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37} to guarantee
 *    correctness for all N < 2^64.
 *
 * Time Complexity: O(N^1/4)
 * Space Complexity: O(1)
 */

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <iomanip>

class PollardRhoFactorizer {
private:
    static unsigned long long mulMod(unsigned long long a, unsigned long long b, unsigned long long mod) {
        return static_cast<unsigned long long>((static_cast<__int128>(a) * b) % mod);
    }

    static unsigned long long powerMod(unsigned long long base, unsigned long long exp, unsigned long long mod) {
        unsigned long long res = 1;
        base %= mod;
        while (exp > 0) {
            if (exp & 1) res = mulMod(res, base, mod);
            base = mulMod(base, base, mod);
            exp >>= 1;
        }
        return res;
    }

    static unsigned long long gcd(unsigned long long a, unsigned long long b) {
        while (b) {
            a %= b;
            std::swap(a, b);
        }
        return a;
    }

public:
    /**
     * @brief Deterministic Miller-Rabin primality test for 64-bit integers.
     */
    static bool isPrime(unsigned long long n) {
        if (n < 2) return false;
        if (n == 2 || n == 3 || n == 5 || n == 7) return true;
        if (n % 2 == 0 || n % 3 == 0 || n % 5 == 0 || n % 7 == 0) return false;

        unsigned long long d = n - 1;
        int s = 0;
        while ((d & 1) == 0) {
            d >>= 1;
            s++;
        }

        static const unsigned long long witnesses[] = {
            2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37
        };

        for (unsigned long long a : witnesses) {
            if (n <= a) break;
            unsigned long long x = powerMod(a, d, n);
            if (x == 1 || x == n - 1) continue;

            bool composite = true;
            for (int r = 1; r < s; ++r) {
                x = mulMod(x, x, n);
                if (x == n - 1) {
                    composite = false;
                    break;
                }
            }
            if (composite) return false;
        }

        return true;
    }

    /**
     * @brief Finds a non-trivial factor of composite n using Pollard's Rho with Brent's acceleration.
     */
    static unsigned long long pollardRho(unsigned long long n) {
        if (n % 2 == 0) return 2;
        if (n % 3 == 0) return 3;

        std::mt19937_64 rng(1337);
        unsigned long long y = 2;
        unsigned long long c = (rng() % (n - 1)) + 1;
        unsigned long long m = 128;
        unsigned long long g = 1;
        unsigned long long r = 1;
        unsigned long long q = 1;
        unsigned long long x = 0;
        unsigned long long ys = 0;

        while (g == 1) {
            x = y;
            for (unsigned long long i = 0; i < r; ++i) {
                y = (mulMod(y, y, n) + c) % n;
            }

            unsigned long long k = 0;
            while (k < r && g == 1) {
                ys = y;
                for (unsigned long long i = 0; i < std::min(m, r - k); ++i) {
                    y = (mulMod(y, y, n) + c) % n;
                    q = mulMod(q, (x > y ? x - y : y - x), n);
                }
                g = gcd(q, n);
                k += m;
            }
            r <<= 1;
        }

        if (g == n) {
            do {
                ys = (mulMod(ys, ys, n) + c) % n;
                g = gcd((x > ys ? x - ys : ys - x), n);
            } while (g == 1);
        }

        return g;
    }

    /**
     * @brief Completely factors n into its prime factors.
     */
    static std::vector<unsigned long long> factorize(unsigned long long n) {
        std::vector<unsigned long long> factors;
        auto recFactor = [&](auto& self, unsigned long long cur) -> void {
            if (cur <= 1) return;
            if (isPrime(cur)) {
                factors.push_back(cur);
                return;
            }
            unsigned long long divisor = pollardRho(cur);
            self(self, divisor);
            self(self, cur / divisor);
        };

        recFactor(recFactor, n);
        std::sort(factors.begin(), factors.end());
        return factors;
    }
};

int main() {
    std::cout << "=== Pollard's Rho Sub-Exponential Integer Factorization ===\n\n";

    std::vector<unsigned long long> testNumbers = {
        1000000007ULL * 1000000009ULL, // Product of two 10-digit primes
        3402823669209384634ULL,
        999999999999999989ULL,
        600851475143ULL,                // Project Euler #3
        9223372036854775783ULL
    };

    for (unsigned long long n : testNumbers) {
        std::cout << "Number: " << n << '\n';
        if (PollardRhoFactorizer::isPrime(n)) {
            std::cout << "  Status: PRIME\n\n";
            continue;
        }

        auto factors = PollardRhoFactorizer::factorize(n);
        std::cout << "  Status: COMPOSITE\n";
        std::cout << "  Prime Factors: ";
        for (size_t i = 0; i < factors.size(); ++i) {
            std::cout << factors[i];
            if (i + 1 < factors.size()) std::cout << " * ";
        }
        std::cout << "\n\n";
    }

    return 0;
}

