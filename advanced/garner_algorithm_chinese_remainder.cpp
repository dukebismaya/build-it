/**
 * @file garner_algorithm_chinese_remainder.cpp
 * @brief Garner's Algorithm for the Chinese Remainder Theorem in C++17.
 *
 * The Chinese Remainder Theorem (CRT) states that given pairwise coprime moduli
 * m_1, m_2, ..., m_k and remainders a_1, a_2, ..., a_k, there exists a unique
 * solution x modulo M = prod(m_i) satisfying:
 *     x = a_i (mod m_i) for all i in [1, k].
 *
 * Direct computation of x involves values up to M, which often exceeds standard
 * 64-bit integer limits. Garner's Algorithm avoids big integer arithmetic by
 * representing x in a mixed-radix number system:
 *     x = x_1 + x_2 * m_1 + x_3 * m_1 * m_2 + ... + x_k * (m_1 * m_2 * ... * m_{k-1})
 * where 0 <= x_i < m_i.
 *
 * Algorithm Steps:
 * 1. Precompute modular inverses r_{i, j} = (m_i)^(-1) mod m_j for all i < j.
 * 2. Sequentially solve for x_i:
 *      x_i = (a_i - (x_1 + x_2 * m_1 + ... + x_{i-1} * (m_1 * ... * m_{i-2}))) * (m_1 * ... * m_{i-1})^(-1) mod m_i.
 * 3. The digits x_i can be converted to any target modulus MOD (e.g. 10^9 + 7) or full integer.
 *
 * Complexity:
 *   - Precomputing inverses: O(k^2 * log(max(m_i)))
 *   - Computing coefficients x_i: O(k^2)
 *   - Space: O(k^2)
 */

#include <iostream>
#include <vector>
#include <numeric>
#include <iomanip>
#include <cstdint>

namespace algebra {

/**
 * @brief Extended Euclidean Algorithm to compute modular inverse.
 * @return Modular inverse of a modulo m (where gcd(a, m) == 1).
 */
int64_t extGCD(int64_t a, int64_t b, int64_t& x, int64_t& y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    int64_t x1, y1;
    int64_t d = extGCD(b, a % b, x1, y1);
    x = y1;
    y = x1 - y1 * (a / b);
    return d;
}

int64_t modInverse(int64_t a, int64_t m) {
    int64_t x, y;
    extGCD(a, m, x, y);
    return (x % m + m) % m;
}

/**
 * @brief Garner's Algorithm implementation.
 * @param a Remainders vector [a_0, a_1, ..., a_{k-1}].
 * @param m Pairwise coprime moduli [m_0, m_1, ..., m_{k-1}].
 * @return Mixed-radix digits [x_0, x_1, ..., x_{k-1}].
 */
std::vector<int64_t> garnerAlgorithm(const std::vector<int64_t>& a, const std::vector<int64_t>& m) {
    size_t k = a.size();
    // Precompute r[i][j] = (m[i])^(-1) mod m[j]
    std::vector<std::vector<int64_t>> r(k, std::vector<int64_t>(k, 0));
    for (size_t i = 0; i < k; ++i) {
        for (size_t j = i + 1; j < k; ++j) {
            r[i][j] = modInverse(m[i], m[j]);
        }
    }

    std::vector<int64_t> x(k);
    for (size_t i = 0; i < k; ++i) {
        x[i] = a[i];
        for (size_t j = 0; j < i; ++j) {
            int64_t diff = (x[i] - x[j]) % m[i];
            if (diff < 0) diff += m[i];
            x[i] = (diff * r[j][i]) % m[i];
        }
    }
    return x;
}

/**
 * @brief Reconstructs the integer solution modulo a target MOD from mixed-radix digits.
 */
int64_t reconstructModulo(const std::vector<int64_t>& x, const std::vector<int64_t>& m, int64_t MOD) {
    int64_t result = 0;
    int64_t mult = 1;
    for (size_t i = 0; i < x.size(); ++i) {
        result = (result + (x[i] % MOD) * mult) % MOD;
        mult = (mult * (m[i] % MOD)) % MOD;
    }
    return result;
}

/**
 * @brief Reconstructs the exact integer solution as __int128 (assuming product fits).
 */
unsigned __int128 reconstructExact(const std::vector<int64_t>& x, const std::vector<int64_t>& m) {
    unsigned __int128 result = 0;
    unsigned __int128 mult = 1;
    for (size_t i = 0; i < x.size(); ++i) {
        result += static_cast<unsigned __int128>(x[i]) * mult;
        mult *= static_cast<unsigned __int128>(m[i]);
    }
    return result;
}

void print128(unsigned __int128 n) {
    if (n == 0) {
        std::cout << "0";
        return;
    }
    std::string s;
    while (n > 0) {
        s += static_cast<char>('0' + (n % 10));
        n /= 10;
    }
    for (auto it = s.rbegin(); it != s.rend(); ++it) {
        std::cout << *it;
    }
}

} // namespace algebra

int main() {
    std::cout << "========================================\n";
    std::cout << "    GARNER'S ALGORITHM FOR THE CRT      \n";
    std::cout << "    (Mixed-Radix Chinese Remainder)     \n";
    std::cout << "========================================\n\n";

    // System of congruences:
    // x = 2 mod 5
    // x = 3 mod 7
    // x = 4 mod 11
    // x = 5 mod 13
    std::vector<int64_t> m = {5, 7, 11, 13};
    std::vector<int64_t> a = {2, 3, 4, 5};

    std::cout << "System of congruences:\n";
    for (size_t i = 0; i < m.size(); ++i) {
        std::cout << "  x = " << a[i] << " (mod " << m[i] << ")\n";
    }

    auto x_digits = algebra::garnerAlgorithm(a, m);

    std::cout << "\nMixed-Radix Representation Digits [x_0, x_1, x_2, x_3]:\n";
    for (size_t i = 0; i < x_digits.size(); ++i) {
        std::cout << "  x_" << i << " = " << x_digits[i] << " (radix basis m_" << i << " = " << m[i] << ")\n";
    }

    unsigned __int128 exactX = algebra::reconstructExact(x_digits, m);
    std::cout << "\nExact Reconstructed Value of x: ";
    algebra::print128(exactX);
    std::cout << "\n";

    int64_t totalMod = 5 * 7 * 11 * 13; // 5005
    std::cout << "Total Modulus M = " << totalMod << "\n\n";

    std::cout << "Verification against each congruence:\n";
    for (size_t i = 0; i < m.size(); ++i) {
        int64_t rem = static_cast<int64_t>(exactX % static_cast<unsigned __int128>(m[i]));
        std::cout << "  " << static_cast<int64_t>(exactX) << " mod " << m[i] << " = " << rem
                  << " (Expected: " << a[i] << ") -> " << (rem == a[i] ? "MATCH" : "FAIL") << "\n";
    }

    // Reconstruction modulo 10^9 + 7
    int64_t targetMod = 1000000007;
    int64_t xMod = algebra::reconstructModulo(x_digits, m, targetMod);
    std::cout << "\nx mod " << targetMod << " = " << xMod << "\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
