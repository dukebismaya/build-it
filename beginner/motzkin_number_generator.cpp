/*
 * ============================================================
 *                Motzkin Number Generator
 * ============================================================
 *
 * Description:
 *   The n-th Motzkin number M(n) counts the ways to draw
 *   non-intersecting chords between n points on a circle
 *   (not necessarily touching every point). Equivalently, it
 *   counts lattice paths from (0,0) to (n,0) using steps
 *   U=(1,1), D=(1,-1), F=(1,0) that never go below the x-axis.
 *
 *   1, 1, 2, 4, 9, 21, 51, 127, 323, 835, 2188, ...
 *
 * Recurrence:
 *   M(n) = M(n-1) + sum_{k=0}^{n-2} M(k) * M(n-2-k)
 *   or, faster:
 *   M(n) = ((2n+1) M(n-1) + (3n-3) M(n-2)) / (n+2)
 *
 * Complexity: O(N) with the linear recurrence.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

namespace motzkin {

    std::vector<uint64_t> linear(int n) {
        std::vector<uint64_t> m(n + 1);
        m[0] = 1; if (n >= 1) m[1] = 1;
        for (int i = 2; i <= n; ++i)
            m[i] = ((2ULL * i + 1) * m[i - 1] + (3ULL * i - 3) * m[i - 2]) / (i + 2);
        return m;
    }

    std::vector<uint64_t> convolution(int n) {
        std::vector<uint64_t> m(n + 1, 0);
        m[0] = 1; if (n >= 1) m[1] = 1;
        for (int i = 2; i <= n; ++i) {
            m[i] = m[i - 1];
            for (int k = 0; k <= i - 2; ++k) m[i] += m[k] * m[i - 2 - k];
        }
        return m;
    }

    // Count U/D/F paths by brute force enumeration (3^n)
    uint64_t brute(int n) {
        uint64_t total = 0;
        uint64_t limit = 1;
        for (int i = 0; i < n; ++i) limit *= 3;
        for (uint64_t code = 0; code < limit; ++code) {
            uint64_t c = code; int h = 0; bool ok = true;
            for (int i = 0; i < n && ok; ++i) {
                int s = static_cast<int>(c % 3); c /= 3;
                h += (s == 0) ? 1 : (s == 1) ? -1 : 0;
                if (h < 0) ok = false;
            }
            if (ok && h == 0) ++total;
        }
        return total;
    }

} // namespace motzkin

int main() {
    std::cout << "============================================================\n";
    std::cout << "                 Motzkin Number Generator\n";
    std::cout << "============================================================\n\n";

    const int N = 35;
    auto a = motzkin::linear(N);
    auto b = motzkin::convolution(N);

    bool ok = true;
    for (int i = 0; i <= N; ++i) {
        std::cout << "M(" << std::setw(2) << i << ") = " << a[i] << "\n";
        if (a[i] != b[i]) ok = false;
    }
    std::cout << "\nLinear vs convolution recurrence: " << (ok ? "PASSED" : "FAILED") << "\n";

    bool ok2 = true;
    for (int i = 0; i <= 12; ++i) if (motzkin::brute(i) != a[i]) ok2 = false;
    std::cout << "Lattice-path brute force (n <= 12): " << (ok2 ? "PASSED" : "FAILED") << "\n";
    return 0;
}
