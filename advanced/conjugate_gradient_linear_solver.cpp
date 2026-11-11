/**
 * @file conjugate_gradient_linear_solver.cpp
 * @brief Conjugate Gradient (CG) iterative solver for Symmetric Positive Definite systems in C++17.
 *
 * Overview (Hestenes & Stiefel, 1952):
 *   The Conjugate Gradient method is an iterative algorithm for solving linear systems
 *   A * x = b where matrix A is Symmetric Positive Definite (SPD: A = A^T and x^T*A*x > 0).
 *   It minimizes the quadratic energy form phi(x) = (1/2) * x^T*A*x - b^T*x along
 *   mutually A-conjugate search directions (p_i^T * A * p_j = 0 for i != j).
 *
 * Iterative Update:
 *   Given x_0, r_0 = b - A*x_0, p_0 = r_0:
 *     alpha_k = (r_k^T * r_k) / (p_k^T * A * p_k)
 *     x_{k+1} = x_k + alpha_k * p_k
 *     r_{k+1} = r_k - alpha_k * A * p_k
 *     beta_k  = (r_{k+1}^T * r_{k+1}) / (r_k^T * r_k)
 *     p_{k+1} = r_{k+1} + beta_k * p_k
 *
 * Convergence Properties:
 *   - Converges to the exact solution in at most N iterations in exact arithmetic.
 *   - Error decays at rate ~ ((sqrt(kappa) - 1) / (sqrt(kappa) + 1))^k where
 *     kappa = cond(A) = lambda_max / lambda_min.
 *
 * Complexity:
 *   - Time per iteration: O(N^2) for dense matrices, or O(nnz) for sparse matrices.
 *   - Space: O(N) auxiliary vectors.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>

namespace linalg {

using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;

double dot(const Vector& a, const Vector& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) sum += a[i] * b[i];
    return sum;
}

Vector matVec(const Matrix& A, const Vector& x) {
    size_t n = A.size();
    Vector res(n, 0.0);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            res[i] += A[i][j] * x[j];
        }
    }
    return res;
}

struct CGResult {
    Vector x;
    int iterations;
    double finalResidualNorm;
    bool converged;
};

/**
 * @brief Solves A * x = b for Symmetric Positive Definite matrix A using Conjugate Gradient.
 */
CGResult conjugateGradient(const Matrix& A, const Vector& b, double tol = 1e-10, int maxIter = 1000) {
    size_t n = b.size();
    Vector x(n, 0.0); // Initial guess x_0 = 0
    Vector r = b;     // Since x_0 = 0, r_0 = b - A*x_0 = b
    Vector p = r;

    double rsOld = dot(r, r);
    if (std::sqrt(rsOld) < tol) {
        return {x, 0, std::sqrt(rsOld), true};
    }

    int iter = 0;
    for (; iter < maxIter; ++iter) {
        Vector Ap = matVec(A, p);
        double pAp = dot(p, Ap);
        if (std::abs(pAp) < 1e-18) break;

        double alpha = rsOld / pAp;
        for (size_t i = 0; i < n; ++i) {
            x[i] += alpha * p[i];
            r[i] -= alpha * Ap[i];
        }

        double rsNew = dot(r, r);
        double resNorm = std::sqrt(rsNew);
        if (resNorm < tol) {
            return {x, iter + 1, resNorm, true};
        }

        double beta = rsNew / rsOld;
        for (size_t i = 0; i < n; ++i) {
            p[i] = r[i] + beta * p[i];
        }
        rsOld = rsNew;
    }

    return {x, iter, std::sqrt(rsOld), false};
}

} // namespace linalg

int main() {
    std::cout << "========================================\n";
    std::cout << "  CONJUGATE GRADIENT SPD LINEAR SOLVER  \n";
    std::cout << "========================================\n\n";

    // Build a 6x6 1D Poisson Discrete Laplacian matrix (Symmetric Positive Definite):
    // Tridiagonal with 2.0 on main diagonal and -1.0 on off-diagonals
    const size_t N = 6;
    linalg::Matrix A(N, linalg::Vector(N, 0.0));
    for (size_t i = 0; i < N; ++i) {
        A[i][i] = 2.0;
        if (i > 0) A[i][i - 1] = -1.0;
        if (i + 1 < N) A[i][i + 1] = -1.0;
    }

    // Plant known true solution x_true = [1, 2, 3, 4, 3, 2]
    linalg::Vector xTrue = {1.0, 2.0, 3.0, 4.0, 3.0, 2.0};
    linalg::Vector b = linalg::matVec(A, xTrue);

    std::cout << "Solving 6x6 SPD 1D Laplacian System A * x = b\n";
    std::cout << "RHS Vector b = [ ";
    for (double val : b) std::cout << std::fixed << std::setprecision(1) << val << " ";
    std::cout << "]\n\n";

    auto res = linalg::conjugateGradient(A, b, 1e-12, 100);

    std::cout << "Solver Status:\n";
    std::cout << "  - Converged?          : " << (res.converged ? "YES" : "NO") << "\n";
    std::cout << "  - Iterations Taken    : " << res.iterations << " (<= N = " << N << " guaranteed)\n";
    std::cout << "  - Final Residual ||r||: " << std::scientific << res.finalResidualNorm << "\n\n";

    std::cout << std::left << std::setw(8) << "Index"
              << std::setw(16) << "CG Solution"
              << std::setw(16) << "True Solution"
              << "Abs Error\n";
    std::cout << std::string(52, '-') << "\n";

    for (size_t i = 0; i < N; ++i) {
        double err = std::abs(res.x[i] - xTrue[i]);
        std::cout << std::left << std::setw(8) << i
                  << std::fixed << std::setprecision(8) << std::setw(16) << res.x[i]
                  << std::setw(16) << xTrue[i]
                  << std::scientific << std::setprecision(3) << err << "\n";
    }

    std::cout << "\nProgram finished successfully.\n";
    return 0;
}
