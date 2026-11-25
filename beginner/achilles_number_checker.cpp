/*
 * ============================================================
 *             Achilles and Strong Achilles Numbers
 * ============================================================
 *
 * Description:
 *   An Achilles number (named by Henry Bottomley after Achilles,
 *   who was powerful but had a fatal flaw) is a positive integer
 *   that is:
 *     1. Powerful (for every prime factor p of n, p^2 divides n),
 *        meaning all exponents in the prime factorization are >= 2.
 *     2. Imperfect (not a perfect power m^k for any m > 1, k > 1),
 *        meaning gcd(e_1, e_2, ..., e_r) = 1.
 *
 *   First few Achilles numbers (OEIS A052486):
 *     72, 108, 200, 288, 392, 432, 500, 648, 675, 800, 864, ...
 *   For example: 72 = 2^3 * 3^2 (exponents 3 and 2 are >= 2 and
 *   gcd(3, 2) = 1).
 *
 *   A Strong Achilles number is an Achilles number n whose Euler
 *   totient phi(n) is ALSO an Achilles number.
 *     First few: 500, 864, 1944, 2000, 2592, 3456, 5000, ...
 *
 * Complexity:
 *   Factorization check per n: O(sqrt(n))
 *   Sieve-based SPF check per n: O(log n) after O(L) sieve.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <numeric>
#include <iomanip>

namespace achilles {

    struct Factor { int p, e; };

    std::vector<Factor> factorize(int n) {
        std::vector<Factor> f;
        for (int p = 2; p * p <= n; ++p) {
            if (n % p == 0) {
                int e = 0;
                while (n % p == 0) { n /= p; ++e; }
                f.push_back({p, e});
            }
        }
        if (n > 1) f.push_back({n, 1});
        return f;
    }

    bool is_powerful(int n) {
        if (n < 1) return false;
        for (auto [p, e] : factorize(n))
            if (e < 2) return false;
        return true;
    }

    bool is_achilles(int n) {
        if (n <= 1) return false;
        auto f = factorize(n);
        int g = 0;
        for (auto [p, e] : f) {
            if (e < 2) return false;
            g = std::gcd(g, e);
        }
        return g == 1;
    }

    int euler_phi(int n) {
        int res = n;
        for (auto [p, e] : factorize(n))
            res = res / p * (p - 1);
        return res;
    }

    bool is_strong_achilles(int n) {
        return is_achilles(n) && is_achilles(euler_phi(n));
    }

} // namespace achilles

int main() {
    using namespace achilles;
    std::cout << "============================================================\n";
    std::cout << "           Achilles and Strong Achilles Numbers\n";
    std::cout << "============================================================\n\n";

    std::vector<int> ach, strong;
    for (int n = 1; n <= 20000; ++n) {
        if (is_achilles(n)) {
            ach.push_back(n);
            if (is_achilles(euler_phi(n))) strong.push_back(n);
        }
    }

    std::cout << "Achilles numbers <= 5000 (" << ach.size() << " <= 20000):\n";
    int printed = 0;
    for (int x : ach) {
        if (x > 5000) break;
        std::cout << std::setw(6) << x;
        if (++printed % 10 == 0) std::cout << "\n";
    }
    if (printed % 10 != 0) std::cout << "\n";

    std::cout << "\nStrong Achilles numbers <= 20000 (" << strong.size() << "):\n";
    for (size_t i = 0; i < strong.size(); ++i) {
        int n = strong[i];
        std::cout << "  " << std::setw(5) << n << "  (phi = " << std::setw(5) << euler_phi(n) << ")\n";
    }

    const int expected_ach[] = {72, 108, 200, 288, 392, 432, 500, 648, 675, 800, 864, 968, 972, 1125};
    const int expected_strong[] = {500, 864, 1944, 2000, 2592, 3456, 5000, 10125, 10368};
    bool ok = true;
    for (int i = 0; i < 14; ++i) if (ach[i] != expected_ach[i]) ok = false;
    for (int i = 0; i < 9; ++i) if (strong[i] != expected_strong[i]) ok = false;

    // Also verify consecutive Achilles pair (5425069447, 5425069448 is famous; check smaller properties)
    for (int x : ach) if (!is_powerful(x)) ok = false;
    std::cout << "\nVerification against OEIS A052486 and A194085: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
