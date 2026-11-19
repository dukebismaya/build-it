/*
 * ============================================================
 *           Pollard's p-1 Factoring Algorithm
 * ============================================================
 *
 * Description:
 *   Pollard's p-1 method is a special-purpose integer
 *   factorization algorithm that finds a prime factor p of a
 *   composite number N if (p-1) is B-smooth (all prime factors
 *   of p-1 are at most B).
 *
 *   The algorithm exploits Fermat's little theorem:
 *     a^(p-1) ≡ 1 (mod p)
 *
 *   If M is a multiple of (p-1) — which happens when (p-1)
 *   is B-smooth and M = lcm(1,2,...,B) — then:
 *     a^M ≡ 1 (mod p)
 *   So p divides gcd(a^M - 1, N).
 *
 * Algorithm:
 *   1. Choose a base a (typically a = 2) and bound B
 *   2. Compute M = a
 *   3. For each prime q <= B:
 *      a. Compute q^k where q^k <= B (highest power)
 *      b. M = M^(q^k) mod N
 *   4. g = gcd(M - 1, N)
 *   5. If 1 < g < N: found factor g
 *      If g == 1: increase B and retry
 *      If g == N: try different base a or use Stage 2
 *
 * Stage 2 (Large Prime Variation):
 *   If (p-1) has one large prime factor between B1 and B2,
 *   we can extend the search efficiently.
 *
 * Complexity:
 *   Time:  O(B * log(N) * log(log(N))) for Stage 1
 *   Space: O(sqrt(B2)) for Stage 2 precomputation
 *
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <algorithm>

namespace pollard_pm1 {

    using u64 = uint64_t;
    using u128 = __uint128_t;

    // Modular multiplication (safe for 64-bit)
    u64 mulmod(u64 a, u64 b, u64 m) {
        return static_cast<u64>(static_cast<u128>(a) * b % m);
    }

    // Modular exponentiation
    u64 powmod(u64 base, u64 exp, u64 mod) {
        u64 result = 1;
        base %= mod;
        while (exp > 0) {
            if (exp & 1) result = mulmod(result, base, mod);
            base = mulmod(base, base, mod);
            exp >>= 1;
        }
        return result;
    }

    // GCD
    u64 gcd(u64 a, u64 b) {
        while (b) {
            a %= b;
            u64 tmp = a;
            a = b;
            b = tmp;
        }
        return a;
    }

    // Simple sieve of Eratosthenes for primes up to limit
    std::vector<u64> sieve_primes(u64 limit) {
        std::vector<bool> is_prime(limit + 1, true);
        is_prime[0] = is_prime[1] = false;
        for (u64 i = 2; i * i <= limit; ++i) {
            if (is_prime[i]) {
                for (u64 j = i * i; j <= limit; j += i) {
                    is_prime[j] = false;
                }
            }
        }
        std::vector<u64> primes;
        for (u64 i = 2; i <= limit; ++i) {
            if (is_prime[i]) primes.push_back(i);
        }
        return primes;
    }

    // Pollard p-1 Stage 1
    // Returns a non-trivial factor of N, or 0 if none found
    u64 pollard_pm1_stage1(u64 N, u64 B1, u64 base = 2) {
        if (N <= 1) return 0;
        if (N % 2 == 0) return 2;

        auto primes = sieve_primes(B1);
        u64 M = base;

        for (u64 q : primes) {
            // Find highest power of q that is <= B1
            u64 pk = q;
            while (pk <= B1 / q) pk *= q;  // avoid overflow
            M = powmod(M, pk, N);

            // Early termination check (optional, every few primes)
        }

        u64 g = gcd((M > 0 ? M - 1 : N - 1), N);
        if (g > 1 && g < N) return g;
        return 0;  // failure
    }

    // Pollard p-1 with Stage 2 (large prime variation)
    u64 pollard_pm1_stage2(u64 N, u64 B1, u64 B2, u64 base = 2) {
        if (N <= 1) return 0;
        if (N % 2 == 0) return 2;

        auto primes = sieve_primes(B2);

        // Stage 1
        u64 M = base;
        for (u64 q : primes) {
            if (q > B1) break;
            u64 pk = q;
            while (pk <= B1 / q) pk *= q;
            M = powmod(M, pk, N);
        }

        u64 g = gcd((M > 0 ? M - 1 : N - 1), N);
        if (g > 1 && g < N) return g;
        if (g == N) return 0;  // bad base

        // Stage 2: check primes between B1 and B2
        // M is already a^(lcm stuff), now compute M^q for each prime q in (B1, B2]
        // Using baby-step-giant-step style optimization
        u64 prev_q = 0;
        u64 Q = M;  // current value

        // Precompute M^(2d) for small differences d
        // Primes > 2 have odd gaps, so differences are even
        std::vector<u64> powers(256, 1);
        for (int d = 1; d < 256; ++d) {
            powers[d] = powmod(M, static_cast<u64>(d), N);
        }

        u64 product = 1;
        int batch_count = 0;

        for (u64 q : primes) {
            if (q <= B1) {
                prev_q = q;
                Q = powmod(M, q, N);
                continue;
            }

            // Compute M^q = M^prev_q * M^(q - prev_q)
            u64 diff = q - prev_q;
            if (diff < 256) {
                Q = mulmod(Q, powers[diff], N);
            } else {
                Q = powmod(M, q, N);
            }
            prev_q = q;

            // Accumulate (Q - 1) into product, check GCD periodically
            u64 term = (Q > 0 ? Q - 1 : N - 1);
            product = mulmod(product, term, N);

            if (++batch_count >= 100) {
                g = gcd(product, N);
                if (g > 1 && g < N) return g;
                if (g == N) return 0;
                batch_count = 0;
            }
        }

        // Final GCD check
        g = gcd(product, N);
        if (g > 1 && g < N) return g;
        return 0;
    }

    // Wrapper: try multiple bounds
    u64 factorize(u64 N) {
        if (N <= 1) return 0;
        if (N % 2 == 0) return 2;
        if (N % 3 == 0) return 3;

        // Try increasing B1 bounds
        std::vector<u64> bounds = {100, 1000, 10000, 100000, 1000000};
        for (u64 B1 : bounds) {
            u64 B2 = B1 * 100;
            u64 factor = pollard_pm1_stage2(N, B1, B2);
            if (factor > 0) return factor;
        }
        return 0;
    }

} // namespace pollard_pm1

int main() {
    using namespace pollard_pm1;

    std::cout << "============================================================\n";
    std::cout << "          Pollard's p-1 Factoring Algorithm\n";
    std::cout << "============================================================\n\n";

    // Test cases: composites where p-1 is smooth
    struct TestCase {
        u64 N;
        std::string note;
    };

    std::vector<TestCase> tests = {
        {15,          "3 * 5 (trivial)"},
        {1073741789ULL * 7, "7 * large_prime"},
        {2039ULL * 2063, "2039 * 2063 (both primes)"},
        {1000003ULL * 7, "7 * 1000003"},
        {40961ULL * 65537, "40961 * 65537 (Fermat prime)"},
        {999961ULL * 999979, "two large primes"},
        {2ULL * 3 * 5 * 7 * 11 * 13 * 17 * 19 * 23, "highly composite"},
        {1234567891ULL * 3, "3 * 1234567891"},
    };

    std::cout << std::left;
    std::cout << std::setw(22) << "N"
              << std::setw(15) << "Factor Found"
              << std::setw(15) << "Co-factor"
              << "Note\n";
    std::cout << std::string(70, '-') << "\n";

    for (const auto& tc : tests) {
        u64 factor = factorize(tc.N);
        if (factor > 0 && factor < tc.N) {
            u64 cofactor = tc.N / factor;
            std::cout << std::setw(22) << tc.N
                      << std::setw(15) << factor
                      << std::setw(15) << cofactor
                      << tc.note << "\n";
        } else {
            std::cout << std::setw(22) << tc.N
                      << std::setw(15) << "FAILED"
                      << std::setw(15) << "-"
                      << tc.note << "\n";
        }
    }

    // Detailed trace for a specific number
    std::cout << "\n--- Stage 1 with increasing B for N = 2039 * 2063 ---\n";
    u64 N_trace = 2039ULL * 2063;
    std::cout << "N = " << N_trace << "\n";
    for (u64 B : {10ULL, 50ULL, 100ULL, 500ULL, 1000ULL, 5000ULL, 10000ULL}) {
        u64 f = pollard_pm1_stage1(N_trace, B);
        std::cout << "  B1 = " << std::setw(6) << B << ": "
                  << (f > 0 ? "factor = " + std::to_string(f) : "not found") << "\n";
    }

    // Explain why it works
    std::cout << "\n--- Why it works for p=2039 ---\n";
    std::cout << "  2039 is prime, 2039 - 1 = 2038 = 2 * 1019\n";
    std::cout << "  1019 is prime, so (p-1) is 1019-smooth\n";
    std::cout << "  B1 >= 1019 should find factor 2039\n";

    std::cout << "\n============================================================\n";
    std::cout << "  Pollard p-1 is effective when N has a prime factor p\n";
    std::cout << "  such that (p-1) is smooth (all prime factors small).\n";
    std::cout << "============================================================\n";

    return 0;
}
