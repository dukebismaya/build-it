/*
 * ============================================================
 *              Large Schroder Number Generator
 * ============================================================
 *
 * Description:
 *   The large Schroder number S(n) counts lattice paths from
 *   (0,0) to (n,n) using steps E=(1,0), N=(0,1), D=(1,1) that
 *   never rise above the diagonal y = x.
 *
 *   1, 2, 6, 22, 90, 394, 1806, 8558, 41586, ...
 *
 *   The little Schroder (super-Catalan) numbers s(n) = S(n)/2
 *   for n >= 1 count bracketings of a sequence of n+1 symbols.
 *
 * Recurrence:
 *   S(n) = ((6n - 3) S(n-1) - (n - 2) S(n-2)) / (n + 1)
 *
 * Verification:
 *   A grid DP over (x, y) with y <= x counts the paths directly.
 *
 * Complexity: O(N) via the recurrence, O(N^2) for the grid DP.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

namespace schroder {

    std::vector<uint64_t> recurrence(int n) {
        std::vector<uint64_t> s(n + 1);
        s[0] = 1; if (n >= 1) s[1] = 2;
        for (int i = 2; i <= n; ++i)
            s[i] = ((6ULL * i - 3) * s[i - 1] - (uint64_t)(i - 2) * s[i - 2]) / (i + 1);
        return s;
    }

    uint64_t grid_paths(int n) {
        std::vector<std::vector<uint64_t>> dp(n + 1, std::vector<uint64_t>(n + 1, 0));
        dp[0][0] = 1;
        for (int x = 0; x <= n; ++x)
            for (int y = 0; y <= x; ++y) {
                if (x == 0 && y == 0) continue;
                uint64_t v = 0;
                if (x > 0 && y <= x - 1) v += dp[x - 1][y];   // E step
                if (y > 0) v += dp[x][y - 1];                 // N step
                if (x > 0 && y > 0) v += dp[x - 1][y - 1];    // D step
                dp[x][y] = v;
            }
        return dp[n][n];
    }

} // namespace schroder

int main() {
    std::cout << "============================================================\n";
    std::cout << "              Large Schroder Number Generator\n";
    std::cout << "============================================================\n\n";

    const int N = 25;
    auto s = schroder::recurrence(N);
    bool ok = true;
    std::cout << std::setw(4) << "n" << std::setw(22) << "Large S(n)" << std::setw(22) << "Little s(n)" << "\n";
    for (int i = 0; i <= N; ++i) {
        std::cout << std::setw(4) << i << std::setw(22) << s[i]
                  << std::setw(22) << (i == 0 ? 1 : s[i] / 2) << "\n";
        if (i <= 18 && schroder::grid_paths(i) != s[i]) ok = false;
    }
    std::cout << "\nRecurrence vs lattice-path grid DP (n <= 18): " << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
