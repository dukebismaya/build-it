/*
 * Program Description:
 * Chinese Remainder Theorem (CRT) Solver in C++.
 * Solves a system of simultaneous modular linear congruences:
 * x ≡ a_0 (mod m_0)
 * x ≡ a_1 (mod m_1)
 * ...
 * x ≡ a_{k-1} (mod m_{k-1})
 *
 * Requirements:
 * The moduli m_0, m_1, ..., m_{k-1} must be pairwise coprime (gcd(m_i, m_j) = 1 for i != j).
 *
 * Algorithm:
 * 1. Let M = m_0 * m_1 * ... * m_{k-1}.
 * 2. For each i, compute M_i = M / m_i.
 * 3. Find modular inverse y_i such that (M_i * y_i) ≡ 1 (mod m_i) via Extended Euclidean.
 * 4. Solution x = sum(a_i * M_i * y_i) mod M.
 *
 * Time Complexity: O(K * log(max(m_i))) where K is the number of congruences.
 */

#include <iostream>
#include <vector>
#include <numeric>

// Extended Euclidean algorithm to compute gcd and modular inverse
long long extGCD(long long a, long long b, long long& x, long long& y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    long long x1, y1;
    long long d = extGCD(b, a % b, x1, y1);
    x = y1;
    y = x1 - y1 * (a / b);
    return d;
}

// Modular inverse: returns x such that (a * x) % m == 1
long long modInverse(long long a, long long m) {
    long long x, y;
    long long g = extGCD(a, m, x, y);
    if (g != 1) return -1; // Inverse does not exist
    return (x % m + m) % m;
}

// Solves system of congruences using Chinese Remainder Theorem
long long solveCRT(const std::vector<long long>& a, const std::vector<long long>& m) {
    int k = a.size();
    long long M = 1;
    for (long long mod : m) {
        M *= mod;
    }

    long long result = 0;
    for (int i = 0; i < k; ++i) {
        long long Mi = M / m[i];
        long long yi = modInverse(Mi, m[i]);
        if (yi == -1) {
            std::cout << "Error: Moduli are not pairwise coprime.\n";
            return -1;
        }

        long long term = (a[i] * Mi) % M;
        term = (term * yi) % M;
        result = (result + term) % M;
    }

    return (result % M + M) % M;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "    Chinese Remainder Theorem (CRT)       \n";
    std::cout << "==========================================\n\n";

    // System of congruences:
    // x ≡ 2 (mod 3)
    // x ≡ 3 (mod 5)
    // x ≡ 2 (mod 7)
    // Famous Sunzi problem from 3rd century AD!
    std::vector<long long> rem = {2, 3, 2};
    std::vector<long long> mod = {3, 5, 7};

    std::cout << "Solving Sun Tzu's classic puzzle:\n";
    for (size_t i = 0; i < rem.size(); ++i) {
        std::cout << "  x ≡ " << rem[i] << " (mod " << mod[i] << ")\n";
    }

    long long M = 3 * 5 * 7;
    long long x = solveCRT(rem, mod);

    std::cout << "\nUnique solution modulo " << M << " is: x = " << x << "\n\n";

    std::cout << "--- Verification ---\n";
    bool allValid = true;
    for (size_t i = 0; i < rem.size(); ++i) {
        long long calculatedRem = x % mod[i];
        std::cout << "  " << x << " % " << mod[i] << " = " << calculatedRem
                  << " (Expected " << rem[i] << ")"
                  << (calculatedRem == rem[i] ? " [PASS]" : " [FAIL]") << "\n";
        if (calculatedRem != rem[i]) allValid = false;
    }

    std::cout << "\nOverall Result: " << (allValid ? "ALL CONGRUENCES SATISFIED" : "ERROR") << "\n";

    return 0;
}
