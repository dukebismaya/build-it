/*
 * ============================================================
 *            Eulerian Number Triangle Generator
 * ============================================================
 *
 * Description:
 *   The Eulerian number A(n, k) (often denoted <n \ k>) counts
 *   permutations of {1, 2, ..., n} that have exactly k ascents
 *   (positions i where p_i < p_{i+1}).
 *
 *   Recurrence:
 *     A(n, k) = (n - k) * A(n - 1, k - 1) + (k + 1) * A(n - 1, k)
 *   with base case A(0, 0) = 1 (and A(n, 0) = A(n, n-1) = 1 for n >= 1).
 *
 *   Identities verified in this program:
 *     1. Symmetry: A(n, k) = A(n, n - 1 - k) for n >= 1.
 *     2. Row sum:  sum_{k=0}^{n-1} A(n, k) = n!
 *     3. Worpitzky's Identity:
 *          x^n = sum_{k=0}^{n-1} A(n, k) * C(x + k, n)
 *
 * Complexity:
 *   Time:  O(N^2) to build the first N rows
 *   Space: O(N^2)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

namespace eulerian {

    using u64 = uint64_t;

    std::vector<std::vector<u64>> build_triangle(int max_n) {
        std::vector<std::vector<u64>> A(max_n + 1);
        A[0] = {1};
        for (int n = 1; n <= max_n; ++n) {
            A[n].assign(n, 0);
            A[n][0] = 1;
            for (int k = 1; k < n; ++k) {
                u64 prev_km1 = A[n - 1][k - 1];
                u64 prev_k   = (k < static_cast<int>(A[n - 1].size())) ? A[n - 1][k] : 0;
                A[n][k] = (n - k) * prev_km1 + (k + 1) * prev_k;
            }
        }
        return A;
    }

    u64 binom(u64 n, u64 k) {
        if (k > n) return 0;
        if (k > n - k) k = n - k;
        u64 r = 1;
        for (u64 i = 1; i <= k; ++i) r = r * (n - k + i) / i;
        return r;
    }

    u64 int_pow(u64 base, int exp) {
        u64 r = 1;
        for (int i = 0; i < exp; ++i) r *= base;
        return r;
    }

} // namespace eulerian

int main() {
    using namespace eulerian;
    std::cout << "============================================================\n";
    std::cout << "            Eulerian Number Triangle Generator\n";
    std::cout << "============================================================\n\n";

    const int N = 10;
    auto A = build_triangle(N);

    std::cout << "Eulerian number triangle A(n, k) for 1 <= n <= 9:\n";
    u64 fact = 1;
    bool ok = true;
    for (int n = 1; n <= 9; ++n) {
        fact *= n;
        std::cout << "n=" << n << ": ";
        u64 sum = 0;
        for (int k = 0; k < n; ++k) {
            sum += A[n][k];
            std::cout << std::setw(7) << A[n][k];
            if (A[n][k] != A[n][n - 1 - k]) ok = false;
        }
        std::cout << "  | sum = " << sum << "\n";
        if (sum != fact) ok = false;
    }

    // Verify Worpitzky's identity: x^n == sum_{k=0}^{n-1} A(n, k) * C(x + k, n)
    for (int n = 1; n <= N && ok; ++n) {
        for (u64 x = 1; x <= 15; ++x) {
            u64 rhs = 0;
            for (int k = 0; k < n; ++k) rhs += A[n][k] * binom(x + k, n);
            if (rhs != int_pow(x, n)) ok = false;
        }
    }

    std::cout << "\nRow sums == n!, symmetry & Worpitzky's identity verified: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
