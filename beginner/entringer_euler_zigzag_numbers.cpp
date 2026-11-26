/*
 * ============================================================
 *        Entringer and Euler Zigzag (Up/Down) Numbers
 * ============================================================
 *
 * Description:
 *   An alternating (zigzag) permutation of {1, 2, ..., n} is one
 *   where p_1 > p_2 < p_3 > p_4 < ... (down-up). André's theorem
 *   (1879) states that the count A_n of such permutations satisfies:
 *     sum_{n >= 0} A_n * x^n / n! = sec(x) + tan(x)
 *   First few Euler zigzag numbers A_n (OEIS A000111):
 *     1, 1, 1, 2, 5, 16, 61, 272, 1385, 7936, 50521, ...
 *
 *   The Entringer numbers E(n, k) count down-up permutations of
 *   {1, ..., n + 1} starting with k + 1. They are computed by the
 *   Seidel-Entringer-Arnold boustrophedon (ox-plowing) triangle:
 *     E(0, 0) = 1,  E(n, 0) = 0 for n >= 1
 *     E(n, k) = E(n, k - 1) + E(n - 1, n - k)
 *   and A_n = E(n, n) for n >= 1.
 *
 * Complexity:
 *   Time:  O(N^2) additions (no multiplication or division!)
 *   Space: O(N^2)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <iomanip>

namespace entringer {

    using u64 = uint64_t;

    std::vector<std::vector<u64>> build_triangle(int max_n) {
        std::vector<std::vector<u64>> E(max_n + 1);
        E[0] = {1};
        for (int n = 1; n <= max_n; ++n) {
            E[n].assign(n + 1, 0);
            for (int k = 1; k <= n; ++k)
                E[n][k] = E[n][k - 1] + E[n - 1][n - k];
        }
        return E;
    }

    // Brute-force count of down-up permutations of {1..n}
    u64 brute_zigzag(int n) {
        if (n <= 1) return 1;
        std::vector<int> p(n);
        for (int i = 0; i < n; ++i) p[i] = i + 1;
        u64 cnt = 0;
        do {
            bool ok = true;
            for (int i = 0; i + 1 < n; ++i) {
                if ((i % 2 == 0 && !(p[i] > p[i + 1])) ||
                    (i % 2 == 1 && !(p[i] < p[i + 1]))) {
                    ok = false;
                    break;
                }
            }
            if (ok) ++cnt;
        } while (std::next_permutation(p.begin(), p.end()));
        return cnt;
    }

} // namespace entringer

int main() {
    using namespace entringer;
    std::cout << "============================================================\n";
    std::cout << "        Entringer and Euler Zigzag (Up/Down) Numbers\n";
    std::cout << "============================================================\n\n";

    const int N = 16;
    auto E = build_triangle(N);

    std::cout << "Entringer triangle E(n, k) for 0 <= n <= 8:\n";
    for (int n = 0; n <= 8; ++n) {
        std::cout << "n=" << n << ": ";
        for (int k = 0; k <= n; ++k) std::cout << std::setw(6) << E[n][k];
        std::cout << "\n";
    }

    std::cout << "\nEuler zigzag numbers A_n = E(n, n) (secant / tangent numbers):\n";
    for (int n = 0; n <= N; ++n)
        std::cout << "  A(" << std::setw(2) << n << ") = " << E[n][n] << "\n";

    const u64 expected[] = {1, 1, 1, 2, 5, 16, 61, 272, 1385, 7936, 50521, 353792};
    bool ok = true;
    for (int n = 0; n < 12; ++n) if (E[n][n] != expected[n]) ok = false;
    for (int n = 1; n <= 9 && ok; ++n) if (E[n][n] != brute_zigzag(n)) ok = false;

    std::cout << "\nBoustrophedon triangle vs OEIS A000111 & permutation brute force: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
