/**
 * @file berlekamp_massey_algorithm.cpp
 * @brief Computes the shortest Linear Recurrence (LFSR minimal polynomial) using Berlekamp-Massey in O(N^2).
 *
 * Theory:
 * Invented by Elwyn Berlekamp (1968) and James Massey (1969).
 * Given a sequence s = [s_0, s_1, ..., s_{n-1}] over a finite field F_p,
 * the algorithm finds the shortest linear recurrence relation:
 *   s_i = sum_{j=1}^L c_j * s_{i-j} (mod p)
 * with the minimal possible order L.
 *
 * Applications:
 * 1. Reed-Solomon and BCH error-correcting code decoding.
 * 2. Cryptanalysis of stream ciphers (determining LFSR linear complexity).
 * 3. Computing the N-th term of an unknown sequence in O(L^2 log N) via the Kitamasa method.
 *
 * Time Complexity: O(N^2) field operations
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>
#include <iomanip>

class BerlekampMassey {
private:
    long long MOD;

    long long power(long long base, long long exp) const {
        long long res = 1;
        base %= MOD;
        while (exp > 0) {
            if (exp & 1) res = (res * base) % MOD;
            base = (base * base) % MOD;
            exp >>= 1;
        }
        return res;
    }

    long long modInverse(long long n) const {
        return power(n, MOD - 2);
    }

public:
    explicit BerlekampMassey(long long modulo = 1000000007LL) : MOD(modulo) {}

    /**
     * @brief Computes the minimal recurrence relation c_1, c_2, ..., c_L.
     * @param s Sequence of values modulo MOD.
     * @return Vector of coefficients [c_1, c_2, ..., c_L] such that
     *         s_i = sum_{j=1}^L c_j * s_{i-j} (mod MOD).
     */
    std::vector<long long> findLinearRecurrence(const std::vector<long long>& s) const {
        int n = static_cast<int>(s.size());
        // C(x) is the connection polynomial: 1 - sum_{j=1}^L c_j * x^j
        std::vector<long long> C = {1};
        std::vector<long long> B = {1};
        int L = 0;
        int m = 1;
        long long b = 1;

        for (int i = 0; i < n; ++i) {
            // Compute discrepancy d
            long long d = s[i] % MOD;
            for (int j = 1; j <= L; ++j) {
                d = (d + C[j] * s[i - j]) % MOD;
            }
            d = (d % MOD + MOD) % MOD;

            if (d == 0) {
                m++;
            } else {
                std::vector<long long> T = C;
                long long coef = (d * modInverse(b)) % MOD;

                if (C.size() < B.size() + m) {
                    C.resize(B.size() + m, 0);
                }

                for (size_t j = 0; j < B.size(); ++j) {
                    C[j + m] = (C[j + m] - coef * B[j]) % MOD;
                    C[j + m] = (C[j + m] + MOD) % MOD;
                }

                if (2 * L <= i) {
                    L = i + 1 - L;
                    B = T;
                    b = d;
                    m = 1;
                } else {
                    m++;
                }
            }
        }

        // Convert connection polynomial C(x) to recurrence coefficients
        // C(x) = 1 - c_1*x - c_2*x^2 - ... - c_L*x^L
        std::vector<long long> recurrence(L);
        for (int j = 1; j <= L; ++j) {
            recurrence[j - 1] = (MOD - C[j]) % MOD;
        }

        return recurrence;
    }
};

int main() {
    std::cout << "=== Berlekamp-Massey Algorithm (Minimal Linear Recurrence) ===\n\n";

    long long MOD = 1000000007LL;
    BerlekampMassey bm(MOD);

    // Test 1: Fibonacci Sequence (0, 1, 1, 2, 3, 5, 8, 13, 21, 34)
    // Recurrence: s_i = 1*s_{i-1} + 1*s_{i-2} -> [1, 1]
    std::vector<long long> fib = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89};
    std::cout << "Test 1: Fibonacci Sequence:\n  Sequence: [ ";
    for (int i = 0; i < 8; ++i) std::cout << fib[i] << " ";
    std::cout << "... ]\n";

    auto fibRec = bm.findLinearRecurrence(fib);
    std::cout << "  Recovered Recurrence Order (L): " << fibRec.size() << '\n';
    std::cout << "  Coefficients: s_i = ";
    for (size_t i = 0; i < fibRec.size(); ++i) {
        std::cout << fibRec[i] << " * s_{i-" << (i + 1) << "}";
        if (i + 1 < fibRec.size()) std::cout << " + ";
    }
    std::cout << "\n  Expected:     s_i = 1 * s_{i-1} + 1 * s_{i-2}\n\n";

    // Test 2: Tribonacci Sequence (0, 0, 1, 1, 2, 4, 7, 13, 24, 44, 81)
    // Recurrence: s_i = s_{i-1} + s_{i-2} + s_{i-3} -> [1, 1, 1]
    std::vector<long long> trib = {0, 0, 1, 1, 2, 4, 7, 13, 24, 44, 81, 149};
    std::cout << "Test 2: Tribonacci Sequence:\n  Sequence: [ ";
    for (int i = 0; i < 8; ++i) std::cout << trib[i] << " ";
    std::cout << "... ]\n";

    auto tribRec = bm.findLinearRecurrence(trib);
    std::cout << "  Recovered Recurrence Order (L): " << tribRec.size() << '\n';
    std::cout << "  Coefficients: s_i = ";
    for (size_t i = 0; i < tribRec.size(); ++i) {
        std::cout << tribRec[i] << " * s_{i-" << (i + 1) << "}";
        if (i + 1 < tribRec.size()) std::cout << " + ";
    }
    std::cout << "\n  Expected:     s_i = 1 * s_{i-1} + 1 * s_{i-2} + 1 * s_{i-3}\n\n";

    // Test 3: Quadratic sequence s_n = n^2 (0, 1, 4, 9, 16, 25, 36, 49)
    // Recurrence order = 3: s_n = 3*s_{n-1} - 3*s_{n-2} + 1*s_{n-3}
    std::vector<long long> squares = {0, 1, 4, 9, 16, 25, 36, 49, 64, 81};
    std::cout << "Test 3: Squares Sequence (n^2):\n";
    auto sqRec = bm.findLinearRecurrence(squares);
    std::cout << "  Recovered Recurrence Order (L): " << sqRec.size() << '\n';
    std::cout << "  Coefficients (mod 10^9+7): [ ";
    for (long long c : sqRec) std::cout << c << " ";
    std::cout << "]\n  (Corresponds to [3, -3, 1] as 10^9+4 = -3)\n";

    return 0;
}

