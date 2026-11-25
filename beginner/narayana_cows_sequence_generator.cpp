/*
 * ============================================================
 *       Narayana Numbers and Narayana's Cows Sequence
 * ============================================================
 *
 * Description:
 *   This program covers two classic sequences named after
 *   Indian mathematicians:
 *
 *   1. Narayana's Cows Sequence (Narayana Pandita, 1356):
 *      A cow produces one calf every year starting in its 4th
 *      year. The population satisfies the 3rd-order recurrence:
 *        a(0) = a(1) = a(2) = 1
 *        a(n) = a(n-1) + a(n-3)
 *      1, 1, 1, 2, 3, 4, 6, 9, 13, 19, 28, 41, 60, 88, ...
 *      The ratio a(n)/a(n-1) converges to the supergolden ratio
 *      psi ~ 1.465571231876768 (real root of x^3 - x^2 - 1 = 0).
 *
 *   2. Narayana Numbers N(n, k) (T. V. Narayana, 1955):
 *      Count Dyck paths of length 2n with exactly k peaks:
 *        N(n, k) = (1 / n) * C(n, k) * C(n, k - 1),  1 <= k <= n
 *      Each row sums to the n-th Catalan number C_n.
 *
 * Complexity:
 *   O(N) for the cows sequence; O(N^2) for the Narayana triangle.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

namespace narayana {

    using u64 = uint64_t;

    std::vector<u64> cows(int n) {
        std::vector<u64> a(n + 1, 1);
        for (int i = 3; i <= n; ++i) a[i] = a[i - 1] + a[i - 3];
        return a;
    }

    u64 binom(int n, int k) {
        if (k < 0 || k > n) return 0;
        if (k > n - k) k = n - k;
        u64 r = 1;
        for (int i = 1; i <= k; ++i) r = r * (n - k + i) / i;
        return r;
    }

    u64 narayana_num(int n, int k) {
        if (n == 0 && k == 0) return 1;
        if (k < 1 || k > n) return 0;
        return binom(n, k) * binom(n, k - 1) / n;
    }

    u64 catalan(int n) {
        return binom(2 * n, n) / (n + 1);
    }

} // namespace narayana

int main() {
    using namespace narayana;
    std::cout << "============================================================\n";
    std::cout << "       Narayana Numbers and Narayana's Cows Sequence\n";
    std::cout << "============================================================\n\n";

    auto c = cows(45);
    std::cout << "Narayana's cows a(0..20):\n  ";
    for (int i = 0; i <= 20; ++i) std::cout << c[i] << " ";
    double psi = static_cast<double>(c[45]) / c[44];
    std::cout << "\nRatio a(45)/a(44) = " << std::fixed << std::setprecision(12) << psi
              << "  (psi^3 - psi^2 - 1 = " << (psi * psi * psi - psi * psi - 1.0) << ")\n\n";

    std::cout << "Narayana triangle N(n, k) for 1 <= n <= 9 (row sums = Catalan C_n):\n";
    bool ok = true;
    for (int n = 1; n <= 9; ++n) {
        std::cout << "n=" << n << ": ";
        u64 row_sum = 0;
        for (int k = 1; k <= n; ++k) {
            u64 v = narayana_num(n, k);
            row_sum += v;
            std::cout << std::setw(5) << v;
            // Symmetry check: N(n, k) == N(n, n - k + 1)
            if (v != narayana_num(n, n - k + 1)) ok = false;
        }
        std::cout << "   | sum = " << row_sum << "\n";
        if (row_sum != catalan(n)) ok = false;
    }

    std::cout << "\nRow sums == Catalan numbers & symmetry verified: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
