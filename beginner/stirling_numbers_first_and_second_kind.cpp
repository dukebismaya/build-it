/*
 * ============================================================
 *         Stirling Numbers of the First and Second Kind
 * ============================================================
 *
 * Description:
 *   1. Unsigned Stirling Numbers of the First Kind [n \ k]:
 *      Count permutations of n elements with exactly k disjoint
 *      cycles. Recurrence:
 *        [n \ k] = (n - 1) * [n - 1 \ k] + [n - 1 \ k - 1]
 *      Row sum: sum_{k=0}^n [n \ k] = n!
 *
 *   2. Stirling Numbers of the Second Kind {n \ k}:
 *      Count ways to partition a set of n labeled objects into k
 *      non-empty unlabeled subsets. Recurrence:
 *        {n \ k} = k * {n - 1 \ k} + {n - 1 \ k - 1}
 *      Row sum: sum_{k=0}^n {n \ k} = B_n (n-th Bell number).
 *
 *   3. Orthogonality / Inversion Identity:
 *      Signed Stirling numbers s(n, k) = (-1)^{n-k} [n \ k] and
 *      S(n, k) = {n \ k} are inverse lower-triangular matrices:
 *        sum_{k=m}^n (-1)^{n-k} [n \ k] {k \ m} = delta_{n, m}
 *
 * Complexity:
 *   Time:  O(N^2) to build both triangles up to row N
 *   Space: O(N^2)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

namespace stirling {

    using i64 = int64_t;

    std::vector<std::vector<i64>> first_kind_unsigned(int max_n) {
        std::vector<std::vector<i64>> s(max_n + 1, std::vector<i64>(max_n + 1, 0));
        s[0][0] = 1;
        for (int n = 1; n <= max_n; ++n)
            for (int k = 1; k <= n; ++k)
                s[n][k] = (n - 1) * s[n - 1][k] + s[n - 1][k - 1];
        return s;
    }

    std::vector<std::vector<i64>> second_kind(int max_n) {
        std::vector<std::vector<i64>> S(max_n + 1, std::vector<i64>(max_n + 1, 0));
        S[0][0] = 1;
        for (int n = 1; n <= max_n; ++n)
            for (int k = 1; k <= n; ++k)
                S[n][k] = k * S[n - 1][k] + S[n - 1][k - 1];
        return S;
    }

} // namespace stirling

int main() {
    using namespace stirling;
    std::cout << "============================================================\n";
    std::cout << "         Stirling Numbers of the First and Second Kind\n";
    std::cout << "============================================================\n\n";

    const int N = 12;
    auto s1 = first_kind_unsigned(N);
    auto s2 = second_kind(N);

    std::cout << "Unsigned Stirling numbers of the 1st kind [n \\ k] (n <= 8):\n";
    for (int n = 1; n <= 8; ++n) {
        std::cout << "n=" << n << ": ";
        for (int k = 1; k <= n; ++k) std::cout << std::setw(6) << s1[n][k];
        std::cout << "\n";
    }

    std::cout << "\nStirling numbers of the 2nd kind {n \\ k} (n <= 8):\n";
    for (int n = 1; n <= 8; ++n) {
        std::cout << "n=" << n << ": ";
        for (int k = 1; k <= n; ++k) std::cout << std::setw(6) << s2[n][k];
        std::cout << "\n";
    }

    // Verify matrix inversion identity: sum_{k=m}^n (-1)^{n-k} [n \ k] {k \ m} == (n == m)
    bool ok = true;
    i64 fact = 1;
    for (int n = 0; n <= N; ++n) {
        if (n > 0) fact *= n;
        i64 row_sum1 = 0;
        for (int k = 0; k <= n; ++k) row_sum1 += s1[n][k];
        if (row_sum1 != fact) ok = false;

        for (int m = 0; m <= n; ++m) {
            i64 dot = 0;
            for (int k = m; k <= n; ++k) {
                i64 sign = ((n - k) & 1) ? -1 : 1;
                dot += sign * s1[n][k] * s2[k][m];
            }
            if (dot != (n == m ? 1 : 0)) ok = false;
        }
    }

    std::cout << "\nRow sums == n! & matrix inversion identity (n <= 12): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
