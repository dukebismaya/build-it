/*
 * ============================================================
 *      Broken Profile DP: Domino Tiling of an N x M Grid
 * ============================================================
 *
 * Description:
 *   Count the ways to tile an N x M board (with optional
 *   blocked cells) using 1x2 dominoes. Process cells one at a
 *   time in row-major order, keeping a bitmask "profile" of M
 *   bits: bit j says whether the cell in column j of the NEXT
 *   frontier position is already covered by a vertical domino
 *   sticking down from above.
 *
 *   At cell (i, j) with current mask:
 *     - If bit j is set (already covered) or the cell is blocked:
 *         clear bit j and move on.
 *     - Otherwise, either
 *         * place a vertical domino: set bit j (covers (i+1, j))
 *         * place a horizontal domino: requires j+1 < M, cell
 *           (i, j+1) free and bit j+1 clear; set bit j+1
 *
 * Complexity:
 *   O(N * M * 2^M) time, O(2^M) space (choose M = min side).
 *
 * Verification:
 *   Kasteleyn's closed form for full boards:
 *     T(N, M) = prod_{j=1}^{ceil(N/2)} prod_{k=1}^{ceil(M/2)}
 *               (4 cos^2(pi j/(N+1)) + 4 cos^2(pi k/(M+1)))
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <iomanip>

namespace broken_profile {

    // grid[i][j] == '#' means blocked
    uint64_t count_tilings(const std::vector<std::string>& grid) {
        int n = static_cast<int>(grid.size()), m = static_cast<int>(grid[0].size());
        std::vector<uint64_t> dp(1u << m, 0), nxt(1u << m, 0);
        dp[0] = 1;
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < m; ++j) {
                std::fill(nxt.begin(), nxt.end(), 0);
                for (unsigned mask = 0; mask < (1u << m); ++mask) {
                    uint64_t ways = dp[mask];
                    if (!ways) continue;
                    unsigned bit = 1u << j;
                    bool blocked = grid[i][j] == '#';
                    if (mask & bit) {
                        if (!blocked) nxt[mask & ~bit] += ways;   // covered from above
                        continue;
                    }
                    if (blocked) { nxt[mask] += ways; continue; }
                    // vertical
                    if (i + 1 < n && grid[i + 1][j] != '#') nxt[mask | bit] += ways;
                    // horizontal
                    if (j + 1 < m && grid[i][j + 1] != '#' && !(mask & (bit << 1)))
                        nxt[mask | (bit << 1)] += ways;
                }
                dp.swap(nxt);
            }
        return dp[0];
    }

    double kasteleyn(int n, int m) {
        const double PI = std::acos(-1.0);
        double r = 1.0;
        for (int j = 1; j <= (n + 1) / 2; ++j)
            for (int k = 1; k <= (m + 1) / 2; ++k) {
                double a = std::cos(PI * j / (n + 1)), b = std::cos(PI * k / (m + 1));
                r *= 4 * a * a + 4 * b * b;
            }
        return r;
    }

} // namespace broken_profile

int main() {
    using namespace broken_profile;
    std::cout << "============================================================\n";
    std::cout << "     Broken Profile DP: Domino Tiling of an N x M Grid\n";
    std::cout << "============================================================\n\n";

    std::cout << "Tilings of full N x M boards:\n     ";
    for (int m = 1; m <= 8; ++m) std::cout << std::setw(12) << m;
    std::cout << "\n";
    bool ok = true;
    for (int n = 1; n <= 8; ++n) {
        std::cout << std::setw(3) << n << "  ";
        for (int m = 1; m <= 8; ++m) {
            uint64_t c = count_tilings(std::vector<std::string>(n, std::string(m, '.')));
            std::cout << std::setw(12) << c;
            if ((n * m) % 2 == 0 && std::llround(kasteleyn(n, m)) != static_cast<long long>(c)) ok = false;
            if ((n * m) % 2 == 1 && c != 0) ok = false;
        }
        std::cout << "\n";
    }
    std::cout << "\nMatches Kasteleyn's formula: " << (ok ? "PASSED" : "FAILED") << "\n";
    std::cout << "8 x 8 chessboard: " << count_tilings(std::vector<std::string>(8, std::string(8, '.')))
              << " (known: 12988816)\n";

    std::vector<std::string> mutilated(8, std::string(8, '.'));
    mutilated[0][0] = '#'; mutilated[7][7] = '#';
    std::cout << "Mutilated chessboard (opposite corners removed): " << count_tilings(mutilated)
              << " (classic proof: 0)\n";

    std::vector<std::string> custom = {
        "....#.",
        "......",
        ".#....",
        "......",
    };
    std::cout << "Custom 4x6 board with 2 holes: " << count_tilings(custom) << " tilings\n";
    std::cout << "12 x 12 board: " << count_tilings(std::vector<std::string>(12, std::string(12, '.'))) << "\n";
    return 0;
}
