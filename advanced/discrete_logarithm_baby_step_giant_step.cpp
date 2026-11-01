/**
 * @file discrete_logarithm_baby_step_giant_step.cpp
 * @brief Solves the Discrete Logarithm Problem a^x = b (mod p) using Shanks' Baby-Step Giant-Step algorithm.
 *
 * Problem:
 * Given integers a, b, and a prime modulus p such that gcd(a, p) = 1,
 * find an integer x such that:
 *   a^x = b (mod p)
 *
 * Algorithm (Daniel Shanks, 1971):
 * - Let m = ceil(sqrt(p)).
 * - Express x in base m: x = i * m - j, where 1 <= i <= m and 0 <= j < m.
 * - Rewriting:
 *     a^(i * m - j) = b (mod p)
 *     a^(i * m) = b * a^j (mod p)
 *
 * - Baby Steps:
 *     Compute b * a^j (mod p) for j = 0, 1, ..., m - 1.
 *     Store pairs (b * a^j (mod p), j) in a hash table.
 * - Giant Steps:
 *     Compute factor a^m (mod p).
 *     For i = 1, 2, ..., m:
 *       Compute cur = (a^m)^i (mod p).
 *       If cur is found in the hash table with value j:
 *         Return x = i * m - j.
 * - If no match is found, no solution exists.
 *
 * Time Complexity: O(sqrt(p))
 * Space Complexity: O(sqrt(p))
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <iomanip>

class BabyStepGiantStep {
private:
    static long long powerMod(long long base, long long exp, long long mod) {
        long long res = 1;
        base %= mod;
        while (exp > 0) {
            if (exp & 1) res = (__int128)res * base % mod;
            base = (__int128)base * base % mod;
            exp >>= 1;
        }
        return res;
    }

public:
    /**
     * @brief Finds the smallest non-negative integer x such that a^x = b (mod p).
     * @param a Base.
     * @param b Target value.
     * @param p Prime modulus.
     * @return x if solution exists, or -1 otherwise.
     */
    static long long solve(long long a, long long b, long long p) {
        a %= p;
        b %= p;

        if (b == 1) return 0; // a^0 = 1 (mod p)

        long long m = static_cast<long long>(std::ceil(std::sqrt(p)));

        // 1. Baby Steps: map value -> j
        std::unordered_map<long long, long long> babySteps;
        long long cur = b;
        for (long long j = 0; j < m; ++j) {
            babySteps[cur] = j;
            cur = (__int128)cur * a % p;
        }

        // 2. Giant Steps: factor = a^m % p
        long long am = powerMod(a, m, p);
        cur = 1;

        for (long long i = 1; i <= m; ++i) {
            cur = (__int128)cur * am % p;
            auto it = babySteps.find(cur);
            if (it != babySteps.end()) {
                long long ans = i * m - it->second;
                return ans;
            }
        }

        return -1; // No solution
    }
};

int main() {
    std::cout << "=== Shanks' Baby-Step Giant-Step (Discrete Logarithm) ===\n\n";

    // Test cases: (a, b, p, expected x)
    struct TestCase {
        long long a;
        long long b;
        long long p;
    };

    std::vector<TestCase> tests = {
        {2, 13, 101},         // 2^x = 13 (mod 101)
        {3, 13, 17},          // 3^x = 13 (mod 17) -> 3^4 = 81 = 13 (mod 17)
        {5, 22, 97},          // 5^x = 22 (mod 97)
        {10, 22004, 100003},  // Large prime mod
        {2, 7, 11}            // 2^x = 7 (mod 11) -> 2^7 = 128 = 7 (mod 11)
    };

    std::cout << "  " << std::setw(8) << "Base (a)"
              << std::setw(12) << "Target (b)"
              << std::setw(14) << "Modulus (p)"
              << std::setw(14) << "Solution (x)"
              << std::setw(18) << "a^x % p Check" << '\n';
    std::cout << "  " << std::string(66, '-') << '\n';

    for (const auto& t : tests) {
        long long x = BabyStepGiantStep::solve(t.a, t.b, t.p);

        std::cout << "  " << std::setw(8) << t.a
                  << std::setw(12) << t.b
                  << std::setw(14) << t.p;

        if (x != -1) {
            // Verify
            long long check = 1;
            for (long long i = 0; i < x; ++i) {
                check = (check * t.a) % t.p;
            }

            std::cout << std::setw(14) << x
                      << std::setw(18) << (check == t.b ? "PASS" : "FAIL") << '\n';
        } else {
            std::cout << std::setw(14) << "No solution"
                      << std::setw(18) << "-" << '\n';
        }
    }

    return 0;
}

