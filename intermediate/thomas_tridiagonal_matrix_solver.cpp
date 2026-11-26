/*
 * ============================================================
 *       Thomas Algorithm (Tridiagonal & Cyclic Systems)
 * ============================================================
 *
 * Description:
 *   The Thomas algorithm (Llewellyn Thomas, 1949) is a specialized
 *   O(N) Gaussian elimination for tridiagonal linear systems:
 *     a_i x_{i-1} + b_i x_i + c_i x_{i+1} = d_i,   i = 0..N-1
 *   (with a_0 = 0, c_{N-1} = 0).
 *
 *   1. Forward sweep:
 *        c'_0 = c_0 / b_0,   d'_0 = d_0 / b_0
 *        c'_i = c_i / (b_i - a_i c'_{i-1})
 *        d'_i = (d_i - a_i d'_{i-1}) / (b_i - a_i c'_{i-1})
 *   2. Back substitution:
 *        x_{N-1} = d'_{N-1}
 *        x_i = d'_i - c'_i x_{i+1}
 *
 *   For a CYCLIC (periodic) tridiagonal matrix with corner entries
 *   a_0 (at (0, N-1)) and c_{N-1} (at (N-1, 0)), we apply the
 *   Sherman-Morrison rank-1 update formula A = T + u v^T, solving
 *   two standard tridiagonal systems in O(N) time.
 *
 * Complexity:
 *   Time:  O(N) (8N flops for standard, ~16N for cyclic)
 *   Space: O(N)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

namespace thomas_tdma {

    std::vector<double> solve_tridiagonal(const std::vector<double>& a,
                                          const std::vector<double>& b,
                                          const std::vector<double>& c,
                                          const std::vector<double>& d) {
        int n = static_cast<int>(b.size());
        std::vector<double> cp(n, 0.0), dp(n, 0.0), x(n, 0.0);
        cp[0] = c[0] / b[0];
        dp[0] = d[0] / b[0];
        for (int i = 1; i < n; ++i) {
            double denom = b[i] - a[i] * cp[i - 1];
            cp[i] = (i < n - 1) ? c[i] / denom : 0.0;
            dp[i] = (d[i] - a[i] * dp[i - 1]) / denom;
        }
        x[n - 1] = dp[n - 1];
        for (int i = n - 2; i >= 0; --i)
            x[i] = dp[i] - cp[i] * x[i + 1];
        return x;
    }

    // Periodic tridiagonal: a[0] is (0, n-1) corner, c[n-1] is (n-1, 0) corner
    std::vector<double> solve_cyclic_tridiagonal(std::vector<double> a,
                                                 std::vector<double> b,
                                                 std::vector<double> c,
                                                 const std::vector<double>& d) {
        int n = static_cast<int>(b.size());
        double gamma = -b[0];
        double alpha = c[n - 1], beta = a[0]; // (n-1, 0) and (0, n-1)
        b[0] -= gamma;
        b[n - 1] -= alpha * beta / gamma;
        a[0] = 0.0;
        c[n - 1] = 0.0;

        auto y = solve_tridiagonal(a, b, c, d);
        std::vector<double> u(n, 0.0);
        u[0] = gamma;
        u[n - 1] = alpha;
        auto z = solve_tridiagonal(a, b, c, u);

        double fact = (y[0] + beta * y[n - 1] / gamma) / (1.0 + z[0] + beta * z[n - 1] / gamma);
        std::vector<double> x(n);
        for (int i = 0; i < n; ++i) x[i] = y[i] - fact * z[i];
        return x;
    }

} // namespace thomas_tdma

int main() {
    using namespace thomas_tdma;
    std::cout << "============================================================\n";
    std::cout << "       Thomas Algorithm (Tridiagonal & Cyclic Systems)\n";
    std::cout << "============================================================\n\n";

    // 1D Poisson finite-difference system: -x_{i-1} + 2 x_i - x_{i+1} = 2
    int n_demo = 5;
    std::vector<double> a(n_demo, -1.0), b(n_demo, 2.0), c(n_demo, -1.0), d(n_demo, 2.0);
    a[0] = 0.0; c[n_demo - 1] = 0.0;
    auto x = solve_tridiagonal(a, b, c, d);
    std::cout << "1D Poisson system (-x_{i-1} + 2x_i - x_{i+1} = 2, N=5):\n  x = ";
    for (double v : x) std::cout << v << " ";
    std::cout << "\n";

    std::mt19937 rng(12345);
    bool ok = true;
    for (int trial = 0; trial < 200 && ok; ++trial) {
        int n = 10 + rng() % 200;
        std::vector<double> A(n), B(n), C(n), D(n);
        for (int i = 0; i < n; ++i) {
            A[i] = (static_cast<int>(rng() % 200) - 100) / 100.0;
            C[i] = (static_cast<int>(rng() % 200) - 100) / 100.0;
            B[i] = std::fabs(A[i]) + std::fabs(C[i]) + 1.5; // strictly diagonally dominant
            D[i] = (static_cast<int>(rng() % 1000) - 500) / 50.0;
        }

        // Standard tridiagonal test
        auto A_std = A, C_std = C;
        A_std[0] = 0.0; C_std[n - 1] = 0.0;
        auto xs = solve_tridiagonal(A_std, B, C_std, D);
        for (int i = 0; i < n; ++i) {
            double lhs = B[i] * xs[i] + (i > 0 ? A_std[i] * xs[i - 1] : 0.0)
                                      + (i + 1 < n ? C_std[i] * xs[i + 1] : 0.0);
            if (std::fabs(lhs - D[i]) > 1e-9) ok = false;
        }

        // Cyclic tridiagonal test
        auto xc = solve_cyclic_tridiagonal(A, B, C, D);
        for (int i = 0; i < n; ++i) {
            double lhs = A[i] * xc[(i - 1 + n) % n] + B[i] * xc[i] + C[i] * xc[(i + 1) % n];
            if (std::fabs(lhs - D[i]) > 1e-9) ok = false;
        }
    }

    std::cout << "\nRandom standard & cyclic tridiagonal verification (200 systems): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
