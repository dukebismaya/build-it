/**
 * @file number_theoretic_transform_ntt.cpp
 * @brief Computes Number Theoretic Transform (NTT) for exact integer polynomial convolution modulo 998244353.
 *
 * Theory:
 * The Number Theoretic Transform (NTT) is an analogue of the Discrete Fourier Transform (DFT)
 * over a finite field F_p rather than the complex numbers.
 *
 * Using prime p = 998244353 = 119 * 2^23 + 1, with primitive root g = 3:
 * The n-th root of unity (for n dividing 2^23) is:
 *   omega_n = g^((p - 1) / n) mod p
 *
 * Advantages over FFT:
 * - Completely exact integer arithmetic without any floating-point roundoff errors.
 * - Perfectly suited for large integer multiplication and polynomial algebra in cryptography.
 *
 * Time Complexity: O(N log N)
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

class NumberTheoreticTransform {
public:
    static constexpr long long MOD = 998244353;
    static constexpr long long G = 3; // Primitive root modulo 998244353

    static long long power(long long base, long long exp) {
        long long res = 1;
        base %= MOD;
        while (exp > 0) {
            if (exp & 1) res = (res * base) % MOD;
            base = (base * base) % MOD;
            exp >>= 1;
        }
        return res;
    }

    static long long modInverse(long long n) {
        return power(n, MOD - 2);
    }

    /**
     * @brief In-place Radix-2 Decimation-In-Time NTT.
     * @param a Vector of coefficients modulo MOD (size must be power of 2).
     * @param invert If true, computes Inverse NTT (INTT).
     */
    static void ntt(std::vector<long long>& a, bool invert) {
        int n = static_cast<int>(a.size());

        // 1. Bit-reversal permutation
        for (int i = 1, j = 0; i < n; ++i) {
            int bit = n >> 1;
            for (; j & bit; bit >>= 1) {
                j ^= bit;
            }
            j ^= bit;
            if (i < j) {
                std::swap(a[i], a[j]);
            }
        }

        // 2. Butterfly operations
        for (int len = 2; len <= n; len <<= 1) {
            long long wlen = power(G, (MOD - 1) / len);
            if (invert) {
                wlen = modInverse(wlen);
            }

            for (int i = 0; i < n; i += len) {
                long long w = 1;
                for (int j = 0; j < len / 2; ++j) {
                    long long u = a[i + j];
                    long long v = (a[i + j + len / 2] * w) % MOD;

                    a[i + j] = (u + v >= MOD ? u + v - MOD : u + v);
                    a[i + j + len / 2] = (u - v < 0 ? u - v + MOD : u - v);

                    w = (w * wlen) % MOD;
                }
            }
        }

        if (invert) {
            long long nInv = modInverse(n);
            for (long long& x : a) {
                x = (x * nInv) % MOD;
            }
        }
    }

    /**
     * @brief Multiplies two polynomials modulo MOD in O(N log N).
     */
    static std::vector<long long> multiply(std::vector<long long> a, std::vector<long long> b) {
        if (a.empty() || b.empty()) return {};

        size_t targetSize = a.size() + b.size() - 1;
        int n = 1;
        while (n < static_cast<int>(targetSize)) {
            n <<= 1;
        }

        a.resize(n, 0);
        b.resize(n, 0);

        ntt(a, false);
        ntt(b, false);

        for (int i = 0; i < n; ++i) {
            a[i] = (a[i] * b[i]) % MOD;
        }

        ntt(a, true);
        a.resize(targetSize);
        return a;
    }
};

int main() {
    std::cout << "=== Number Theoretic Transform (NTT modulo 998244353) ===\n\n";

    // Example 1:
    // A(x) = 1 + 2x + 3x^2
    // B(x) = 4 + 5x + 6x^2
    // Product C(x) = 4 + 13x + 28x^2 + 27x^3 + 18x^4
    std::vector<long long> A = {1, 2, 3};
    std::vector<long long> B = {4, 5, 6};

    std::cout << "1. Polynomial A(x): [ ";
    for (long long c : A) std::cout << c << " ";
    std::cout << "]\n   Polynomial B(x): [ ";
    for (long long c : B) std::cout << c << " ";
    std::cout << "]\n";

    auto C = NumberTheoreticTransform::multiply(A, B);

    std::cout << "   Product C(x) via NTT: [ ";
    for (long long c : C) std::cout << c << " ";
    std::cout << "]\n";
    std::cout << "   Expected Result:      [ 4 13 28 27 18 ]\n\n";

    // Example 2: Large polynomial test with modulo wrap
    std::vector<long long> P1 = {100000, 200000, 300000, 400000};
    std::vector<long long> P2 = {500000, 600000, 700000};

    std::cout << "2. Large Coefficient Product mod 998244353:\n";
    auto P3 = NumberTheoreticTransform::multiply(P1, P2);

    std::cout << "   Product coefficients: [ ";
    for (long long c : P3) std::cout << c << " ";
    std::cout << "]\n";

    return 0;
}

