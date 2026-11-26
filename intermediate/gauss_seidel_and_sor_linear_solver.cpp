/*
 * ============================================================
 *       Gauss-Seidel and SOR Iterative Linear Solvers
 * ============================================================
 *
 * Description:
 *   For a linear system A x = b with non-zero diagonal entries:
 *     1. Jacobi Iteration uses only values from the previous step:
 *          x_i^{(k+1)} = (b_i - sum_{j != i} a_{ij} x_j^{(k)}) / a_{ii}
 *
 *     2. Gauss-Seidel Iteration immediately uses newly computed
 *        components x_j^{(k+1)} for j < i:
 *          x_i^{GS} = (b_i - sum_{j < i} a_{ij} x_j^{(k+1)}
 *                          - sum_{j > i} a_{ij} x_j^{(k)}) / a_{ii}
 *
 *     3. Successive Over-Relaxation (SOR) takes a weighted average
 *        with relaxation parameter omega in (0, 2):
 *          x_i^{(k+1)} = (1 - omega) x_i^{(k)} + omega * x_i^{GS}
 *        For omega = 1, SOR reduces to Gauss-Seidel. For symmetric
 *        positive-definite (SPD) matrices, Ostrowski-Reich theorem
 *        guarantees convergence for all omega in (0, 2), and an
 *        optimal omega in (1, 2) dramatically cuts the iteration count.
 *
 * Complexity:
 *   O(N^2) per iteration for dense N x N matrices (O(nnz) for sparse).
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>

namespace sor_solver {

    struct SolveStats {
        std::vector<double> x;
        int iters;
        double residual;
    };

    double max_residual(const std::vector<std::vector<double>>& A,
                        const std::vector<double>& b,
                        const std::vector<double>& x) {
        int n = static_cast<int>(b.size());
        double r = 0.0;
        for (int i = 0; i < n; ++i) {
            double s = -b[i];
            for (int j = 0; j < n; ++j) s += A[i][j] * x[j];
            r = std::max(r, std::fabs(s));
        }
        return r;
    }

    SolveStats jacobi(const std::vector<std::vector<double>>& A,
                      const std::vector<double>& b,
                      double tol = 1e-11, int max_iter = 5000) {
        int n = static_cast<int>(b.size());
        std::vector<double> x(n, 0.0), nxt(n, 0.0);
        for (int it = 1; it <= max_iter; ++it) {
            for (int i = 0; i < n; ++i) {
                double sigma = 0.0;
                for (int j = 0; j < n; ++j) if (j != i) sigma += A[i][j] * x[j];
                nxt[i] = (b[i] - sigma) / A[i][i];
            }
            x.swap(nxt);
            double res = max_residual(A, b, x);
            if (res < tol) return {x, it, res};
        }
        return {x, max_iter, max_residual(A, b, x)};
    }

    SolveStats sor(const std::vector<std::vector<double>>& A,
                   const std::vector<double>& b,
                   double omega = 1.0, double tol = 1e-11, int max_iter = 5000) {
        int n = static_cast<int>(b.size());
        std::vector<double> x(n, 0.0);
        for (int it = 1; it <= max_iter; ++it) {
            for (int i = 0; i < n; ++i) {
                double sigma = 0.0;
                for (int j = 0; j < n; ++j) if (j != i) sigma += A[i][j] * x[j];
                double gs = (b[i] - sigma) / A[i][i];
                x[i] = (1.0 - omega) * x[i] + omega * gs;
            }
            double res = max_residual(A, b, x);
            if (res < tol) return {x, it, res};
        }
        return {x, max_iter, max_residual(A, b, x)};
    }

} // namespace sor_solver

int main() {
    using namespace sor_solver;
    const double PI = std::acos(-1.0);
    std::cout << "============================================================\n";
    std::cout << "       Gauss-Seidel and SOR Iterative Linear Solvers\n";
    std::cout << "============================================================\n\n";

    // Build 1D Laplacian SPD matrix of size N = 25 (theoretical optimal omega = 2 / (1 + sin(pi/(N+1))))
    const int N = 25;
    std::vector<std::vector<double>> A(N, std::vector<double>(N, 0.0));
    std::vector<double> b(N, 1.0);
    for (int i = 0; i < N; ++i) {
        A[i][i] = 2.0;
        if (i > 0) A[i][i - 1] = -1.0;
        if (i + 1 < N) A[i][i + 1] = -1.0;
    }
    double omega_opt = 2.0 / (1.0 + std::sin(PI / (N + 1)));

    auto r_jac = jacobi(A, b);
    auto r_gs  = sor(A, b, 1.0);
    auto r_sor = sor(A, b, omega_opt);

    std::cout << "25x25 Poisson system (tol = 1e-11, optimal omega = "
              << std::fixed << std::setprecision(4) << omega_opt << "):\n";
    std::cout << "  Jacobi             : " << std::setw(5) << r_jac.iters
              << " iterations (res = " << std::scientific << std::setprecision(2) << r_jac.residual << ")\n";
    std::cout << "  Gauss-Seidel (w=1) : " << std::setw(5) << r_gs.iters
              << " iterations (res = " << r_gs.residual << ")\n";
    std::cout << "  SOR (w = w_opt)    : " << std::setw(5) << r_sor.iters
              << " iterations (res = " << r_sor.residual << ")\n";

    bool ok = (r_sor.iters < r_gs.iters && r_gs.iters < r_jac.iters);
    if (r_jac.residual > 1e-10 || r_gs.residual > 1e-10 || r_sor.residual > 1e-10) ok = false;
    for (int i = 0; i < N; ++i)
        if (std::fabs(r_sor.x[i] - r_gs.x[i]) > 1e-8) ok = false;

    std::cout << "\nConvergence hierarchy (SOR < Gauss-Seidel < Jacobi) & residuals < 1e-11: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
