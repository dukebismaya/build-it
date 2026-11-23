/*
 * ============================================================
 *   Lagrange Interpolation for Power Sums  (mod 1e9+7)
 * ============================================================
 *
 * Description:
 *   S_k(n) = 1^k + 2^k + ... + n^k is a polynomial in n of
 *   degree k+1. Given its values at k+2 consecutive points
 *   x = 0, 1, ..., k+1 we can evaluate it at ANY n (even
 *   n ~ 1e18) via Lagrange interpolation:
 *
 *     P(n) = sum_i y_i * prod_{j != i} (n - j) / (i - j)
 *
 *   For equally spaced nodes x_i = i the denominator is
 *     prod_{j != i} (i - j) = i! * (d - i)! * (-1)^(d - i)
 *   and the numerator uses prefix/suffix products of (n - j),
 *   giving an O(d) evaluation instead of O(d^2).
 *
 * Complexity:
 *   O(k log MOD) to build the sample values, O(k) to evaluate.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>

namespace lagrange {

    const int64_t MOD = 1000000007LL;

    int64_t power(int64_t b, int64_t e) {
        int64_t r = 1; b %= MOD; if (b < 0) b += MOD;
        while (e) { if (e & 1) r = r * b % MOD; b = b * b % MOD; e >>= 1; }
        return r;
    }

    // Evaluate the unique degree-(m-1) polynomial with P(i) = y[i], i = 0..m-1, at x
    int64_t interpolate(const std::vector<int64_t>& y, int64_t x) {
        int m = static_cast<int>(y.size());
        x %= MOD;
        if (x < m) return y[x];   // only valid if the true x < m; caller passes x < MOD
        std::vector<int64_t> pre(m + 1, 1), suf(m + 1, 1), fact(m, 1), inv_fact(m, 1);
        for (int i = 0; i < m; ++i) pre[i + 1] = pre[i] * ((x - i + MOD) % MOD) % MOD;
        for (int i = m - 1; i >= 0; --i) suf[i] = suf[i + 1] * ((x - i + MOD) % MOD) % MOD;
        for (int i = 1; i < m; ++i) fact[i] = fact[i - 1] * i % MOD;
        inv_fact[m - 1] = power(fact[m - 1], MOD - 2);
        for (int i = m - 1; i > 0; --i) inv_fact[i - 1] = inv_fact[i] * i % MOD;

        int64_t res = 0;
        for (int i = 0; i < m; ++i) {
            int64_t term = y[i] * pre[i] % MOD * suf[i + 1] % MOD
                         * inv_fact[i] % MOD * inv_fact[m - 1 - i] % MOD;
            if ((m - 1 - i) & 1) term = (MOD - term) % MOD;
            res = (res + term) % MOD;
        }
        return res;
    }

    int64_t power_sum(int64_t n, int k) {
        std::vector<int64_t> y(k + 2, 0);
        for (int i = 1; i <= k + 1; ++i) y[i] = (y[i - 1] + power(i, k)) % MOD;
        return interpolate(y, n);
    }

    int64_t brute(int64_t n, int k) {
        int64_t s = 0;
        for (int64_t i = 1; i <= n; ++i) s = (s + power(i, k)) % MOD;
        return s;
    }

} // namespace lagrange

int main() {
    using namespace lagrange;
    std::cout << "============================================================\n";
    std::cout << "    Lagrange Interpolation for Power Sums (mod 1e9+7)\n";
    std::cout << "============================================================\n\n";

    bool ok = true;
    for (int k = 0; k <= 10; ++k)
        for (int64_t n : {0LL, 1LL, 5LL, 37LL, 1000LL, 54321LL})
            if (power_sum(n, k) != brute(n, k)) ok = false;
    std::cout << "Interpolation vs brute force (k <= 10, n <= 54321): " << (ok ? "PASSED" : "FAILED") << "\n\n";

    // Closed forms: S1 = n(n+1)/2, S2 = n(n+1)(2n+1)/6, S3 = S1^2
    int64_t n = 1000000000000LL % MOD;
    int64_t inv2 = power(2, MOD - 2), inv6 = power(6, MOD - 2);
    int64_t s1 = n * ((n + 1) % MOD) % MOD * inv2 % MOD;
    int64_t s2 = n * ((n + 1) % MOD) % MOD * ((2 * n + 1) % MOD) % MOD * inv6 % MOD;
    int64_t s3 = s1 * s1 % MOD;
    std::cout << "n = 10^12:\n";
    std::cout << "  S1: interp = " << power_sum(1000000000000LL, 1) << ", closed form = " << s1 << "\n";
    std::cout << "  S2: interp = " << power_sum(1000000000000LL, 2) << ", closed form = " << s2 << "\n";
    std::cout << "  S3: interp = " << power_sum(1000000000000LL, 3) << ", closed form = " << s3 << "\n";

    std::cout << "\nLarge examples (n = 10^18):\n";
    for (int k : {10, 100, 1000, 100000})
        std::cout << "  S_" << k << "(10^18) mod p = " << power_sum(1000000000000000000LL, k) << "\n";
    return 0;
}
