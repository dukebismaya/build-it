/**
 * ============================================================================
 * Ulam Spiral (Prime Spiral) Matrix Generator & Diagonal Analysis
 * ============================================================================
 * 
 * Description:
 * The Ulam Spiral (or prime spiral) was discovered by mathematician Stanislaw
 * Ulam in 1963 during a conference. Natural numbers 1, 2, 3, 4, ... are written
 * on a square grid in an outward counter-clockwise spiral starting at the center:
 * 
 * ASCII Illustration (5x5 Spiral):
 *   17  16  15  14  13
 *   18   5   4   3  12
 *   19   6   1   2  11
 *   20   7   8   9  10
 *   21  22  23  24  25
 * 
 * Surprising Property:
 * When prime numbers are marked on the spiral, primes tend to concentrate
 * along certain diagonal lines far more densely than expected by random chance.
 * For instance, polynomials like Euler's f(n) = n^2 + n + 41 produce diagonal
 * lines with unusually rich prime yields.
 * 
 * Features of this program:
 * 1. Generates an odd N x N Ulam spiral grid.
 * 2. Identifies all prime cells.
 * 3. Renders a formatted text visualization of primes (*) vs composites (.).
 * 4. Compares prime density on the main diagonals vs other cells.
 * 
 * Time Complexity:
 * - O(N^2) to generate spiral and test primality up to N^2
 * - Space Complexity: O(N^2) for the grid
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>

namespace ulam {

bool isPrime(int n) {
    if (n < 2) return false;
    for (int d = 2; d * d <= n; ++d) {
        if (n % d == 0) return false;
    }
    return true;
}

/**
 * Builds an N x N Ulam spiral grid (N must be odd).
 * Returns matrix of size N x N with numbers 1 .. N^2.
 */
std::vector<std::vector<int>> buildSpiral(int n) {
    if (n % 2 == 0) n++; // Ensure odd dimension

    std::vector<std::vector<int>> grid(n, std::vector<int>(n, 0));

    int r = n / 2;
    int c = n / 2;
    grid[r][c] = 1;

    // Movement directions: Right, Up, Left, Down
    int dr[] = {0, -1, 0, 1};
    int dc[] = {1, 0, -1, 0};

    int current_val = 2;
    int step_size = 1;
    int dir = 0;

    while (current_val <= n * n) {
        for (int repeat = 0; repeat < 2; ++repeat) {
            for (int s = 0; s < step_size; ++s) {
                if (current_val > n * n) break;
                r += dr[dir];
                c += dc[dir];
                grid[r][c] = current_val++;
            }
            dir = (dir + 1) % 4;
        }
        step_size++;
    }

    return grid;
}

} // namespace ulam

int main() {
    std::cout << "========================================================\n";
    std::cout << " Ulam Spiral (Prime Spiral) Matrix Generator\n";
    std::cout << "========================================================\n\n";

    constexpr int N = 7;
    auto grid = ulam::buildSpiral(N);

    std::cout << "Numerical " << N << "x" << N << " Ulam Spiral:\n";
    for (int r = 0; r < N; ++r) {
        std::cout << "  ";
        for (int c = 0; c < N; ++c) {
            std::cout << std::setw(4) << grid[r][c];
        }
        std::cout << "\n";
    }

    std::cout << "\nPrime Pattern Visualization (* = Prime, . = Composite/1):\n";
    for (int r = 0; r < N; ++r) {
        std::cout << "  ";
        for (int c = 0; c < N; ++c) {
            bool prime = ulam::isPrime(grid[r][c]);
            std::cout << (prime ? " * " : " . ");
        }
        std::cout << "\n";
    }

    // Statistical diagonal analysis on a larger 21x21 spiral
    int large_n = 21;
    auto large_grid = ulam::buildSpiral(large_n);

    int diagonal_primes = 0;
    int diagonal_cells = 0;
    int non_diagonal_primes = 0;
    int non_diagonal_cells = 0;

    for (int r = 0; r < large_n; ++r) {
        for (int c = 0; c < large_n; ++c) {
            int val = large_grid[r][c];
            bool on_diag = (r == c || r + c == large_n - 1);
            bool prime = ulam::isPrime(val);

            if (on_diag) {
                diagonal_cells++;
                if (prime) diagonal_primes++;
            } else {
                non_diagonal_cells++;
                if (prime) non_diagonal_primes++;
            }
        }
    }

    std::cout << "\n--------------------------------------------------------\n";
    std::cout << " Prime Density Analysis (" << large_n << "x" << large_n << " Spiral, max=" << large_n * large_n << "):\n";
    std::cout << "--------------------------------------------------------\n";

    double diag_density = (100.0 * diagonal_primes) / diagonal_cells;
    double non_diag_density = (100.0 * non_diagonal_primes) / non_diagonal_cells;

    std::cout << "Diagonal Cells:     " << diagonal_primes << " / " << diagonal_cells
              << " (" << std::fixed << std::setprecision(2) << diag_density << "% primes)\n";
    std::cout << "Non-Diagonal Cells: " << non_diagonal_primes << " / " << non_diagonal_cells
              << " (" << std::fixed << std::setprecision(2) << non_diag_density << "% primes)\n";
    std::cout << "Diagonal Prime Concentration Ratio: "
              << (diag_density / non_diag_density) << "x higher than off-diagonal!\n";

    return 0;
}
