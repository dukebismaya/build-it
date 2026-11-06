/**
 * @file tonelli_shanks_modular_sqrt.cpp
 * @brief Computes Modular Square Roots (r^2 = n mod p) using the Tonelli-Shanks Algorithm.
 *
 * Problem:
 * Given an integer n and an odd prime p, find an integer r such that:
 *   r^2 = n (mod p)
 *
 * Algorithm (Alberto Tonelli 1891, Daniel Shanks 1973):
 * 1. Euler's Criterion:
 *    Compute Legendre symbol L = n^((p - 1) / 2) mod p.
 *    If L == p - 1 (-1 mod p), n is a quadratic non-residue -> No solution.
 *    If L == 0, r = 0.
 * 2. Factor out powers of 2: p - 1 = Q * 2^S with Q odd.
 * 3. Find a quadratic non-residue z (where z^((p - 1) / 2) = -1 mod p).
 * 4. Initialize variables:
 *      M = S, c = z^Q mod p, t = n^Q mod p, R = n^((Q + 1) / 2) mod p
 * 5. Loop until t = 1 mod p:
 *      Find least i < M such that t^(2^i) = 1 mod p.
 *      b = c^(2^(M - i - 1)) mod p
 *      M = i, c = b^2 mod p, t = t * b^2 mod p, R = R * b mod p
 * 6. Roots are R and p - R.
 *
 * Time Complexity: O(log^2(p)) average time
 * Space Complexity: O(1)
 */

#include <iostream>
#include <vector>
#include <random>
#include <iomanip>

class TonelliShanks {
private:
    static long long powerMod(long long base, long long exp, long long mod) {
        long long res = 1;
        base %= mod;
        while (exp > 0) {
            if (exp & 1) res = (static_cast<__int128>(res) * base) % mod;
            base = (static_cast<__int128>(base) * base) % mod;
            exp >>= 1;
        }
        return res;
    }

public:
    struct SqrtResult {
        bool exists;
        long long root1;
        long long root2;
    };

    /**
     * @brief Computes r such that r^2 = n (mod p).
     */
    static SqrtResult solve(long long n, long long p) {
        n %= p;
        if (n < 0) n += p;
        if (n == 0) return {true, 0, 0};
        if (p == 2) return {true, n, n};

        // 1. Euler's criterion for quadratic residuosity
        if (powerMod(n, (p - 1) / 2, p) != 1) {
            return {false, -1, -1}; // Not a quadratic residue
        }

        // Special fast case: p = 3 (mod 4)
        if (p % 4 == 3) {
            long long r = powerMod(n, (p + 1) / 4, p);
            return {true, r, p - r};
        }

        // 2. Factor p - 1 = Q * 2^S with Q odd
        long long Q = p - 1;
        long long S = 0;
        while ((Q & 1) == 0) {
            Q >>= 1;
            S++;
        }

        // 3. Find a quadratic non-residue z
        long long z = 2;
        while (powerMod(z, (p - 1) / 2, p) == 1) {
            z++;
        }

        // 4. Initialize
        long long M = S;
        long long c = powerMod(z, Q, p);
        long long t = powerMod(n, Q, p);
        long long R = powerMod(n, (Q + 1) / 2, p);

        // 5. Iterative loop
        while (t != 1) {
            // Find lowest i such that t^(2^i) = 1 mod p
            long long i = 0;
            long long temp = t;
            while (temp != 1 && i < M) {
                temp = (static_cast<__int128>(temp) * temp) % p;
                i++;
            }

            if (i == M) return {false, -1, -1}; // Safety check

            long long b = powerMod(c, 1LL << (M - i - 1), p);
            M = i;
            c = (static_cast<__int128>(b) * b) % p;
            t = (static_cast<__int128>(t) * c) % p;
            R = (static_cast<__int128>(R) * b) % p;
        }

        long long r1 = R;
        long long r2 = p - R;
        if (r1 > r2) std::swap(r1, r2);

        return {true, r1, r2};
    }
};

int main() {
    std::cout << "=== Tonelli-Shanks Modular Square Root Algorithm ===\n\n";

    // Test cases: (n, p)
    struct TestCase {
        long long n;
        long long p;
    };

    std::vector<TestCase> testCases = {
        {10, 13},      // 10 mod 13: 6^2 = 36 = 10 mod 13, 7^2 = 49 = 10 mod 13
        {56, 101},     // 56 mod 101
        {1030, 10007}, // p = 10007
        {5, 17},       // 5 mod 17: No solution (Legendre = -1)
        {2, 1000000007} // Square root of 2 modulo 10^9+7
    };

    std::cout << "  " << std::setw(12) << "Target (n)"
              << std::setw(16) << "Prime (p)"
              << std::setw(16) << "Roots (mod p)"
              << std::setw(18) << "Verification" << '\n';
    std::cout << "  " << std::string(64, '-') << '\n';

    for (const auto& tc : testCases) {
        auto res = TonelliShanks::solve(tc.n, tc.p);
        std::cout << "  " << std::setw(12) << tc.n
                  << std::setw(16) << tc.p;

        if (res.exists) {
            std::string rootsStr = std::to_string(res.root1) + ", " + std::to_string(res.root2);
            long long check = (static_cast<__int128>(res.root1) * res.root1) % tc.p;
            std::cout << "  " << std::setw(16) << rootsStr
                      << std::setw(16) << (check == (tc.n % tc.p) ? "PASS (r^2 = n)" : "FAIL") << '\n';
        } else {
            std::cout << "  " << std::setw(16) << "No Solution"
                      << std::setw(16) << "Non-residue" << '\n';
        }
    }

    return 0;
}

