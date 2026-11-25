/*
 * ============================================================
 *                 Delannoy Number Generator
 * ============================================================
 *
 * Description:
 *   The Delannoy number D(m, n) (Henri Delannoy, 1895) counts
 *   lattice paths from (0, 0) to (m, n) using only steps:
 *     - East:       (1, 0)
 *     - North:      (0, 1)
 *     - North-East: (1, 1)
 *
 *   Recurrence:
 *     D(m, n) = D(m - 1, n) + D(m, n - 1) + D(m - 1, n - 1)
 *   with base cases D(m, 0) = D(0, n) = 1.
 *
 *   Closed form:
 *     D(m, n) = sum_{k=0}^{min(m,n)} C(m, k) * C(n, k) * 2^k
 *
 *   The central Delannoy numbers D(n) = D(n, n) satisfy:
 *     n * D(n) = 3(2n - 1) D(n - 1) - (n - 1) D(n - 2)
 *   First few central Delannoy numbers (OEIS A001850):
 *     1, 3, 13, 63, 321, 1683, 8989, 48639, 265729, ...
 *
 * Complexity:
 *   Grid table: O(M * N); Central recurrence: O(N).
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

namespace delannoy {

    using u64 = uint64_t;

    std::vector<std::vector<u64>> table(int m, int n) {
        std::vector<std::vector<u64>> dp(m + 1, std::vector<u64>(n + 1, 1));
        for (int i = 1; i <= m; ++i)
            for (int j = 1; j <= n; ++j)
                dp[i][j] = dp[i - 1][j] + dp[i][j - 1] + dp[i - 1][j - 1];
        return dp;
    }

    std::vector<u64> central_recurrence(int n) {
        std::vector<u64> d(n + 1, 1);
        if (n >= 1) d[1] = 3;
        for (int i = 2; i <= n; ++i)
            d[i] = (3ULL * (2 * i - 1) * d[i - 1] - (i - 1) * d[i - 2]) / i;
        return d;
    }

    u64 closed_form(int m, int n) {
        auto binom = [](int a, int b) -> u64 {
            if (b < 0 || b > a) return 0;
            u64 r = 1;
            for (int i = 1; i <= b; ++i) r = r * (a - b + i) / i;
            return r;
        };
        u64 sum = 0;
        int lim = m < n ? m : n;
        for (int k = 0; k <= lim; ++k)
            sum += binom(m, k) * binom(n, k) * (1ULL << k);
        return sum;
    }

} // namespace delannoy

int main() {
    using namespace delannoy;
    std::cout << "============================================================\n";
    std::cout << "                  Delannoy Number Generator\n";
    std::cout << "============================================================\n\n";

    const int N = 15;
    auto dp = table(N, N);
    auto cent = central_recurrence(N);

    std::cout << "Delannoy grid D(m, n) for 0 <= m, n <= 7:\n";
    for (int i = 0; i <= 7; ++i) {
        for (int j = 0; j <= 7; ++j) std::cout << std::setw(8) << dp[i][j];
        std::cout << "\n";
    }

    std::cout << "\nCentral Delannoy numbers D(n, n):\n";
    bool ok = true;
    for (int i = 0; i <= N; ++i) {
        std::cout << "  D(" << std::setw(2) << i << ") = " << cent[i] << "\n";
        if (cent[i] != dp[i][i]) ok = false;
    }

    for (int i = 0; i <= N && ok; ++i)
        for (int j = 0; j <= N && ok; ++j)
            if (dp[i][j] != closed_form(i, j)) ok = false;

    std::cout << "\nGrid DP == closed form == 3-term central recurrence: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
