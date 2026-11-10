/**
 * @file affine_scaling_interior_point.cpp
 * @brief Dikin's Affine Scaling Interior Point Algorithm for Linear Programming in C++17.
 *
 * Problem Formulation:
 *   Minimize   c^T * x
 *   Subject to A * x = b,  x >= 0
 *   where A is an m x n matrix with m < n.
 *
 * Algorithm Overview (Dikin, 1967):
 *   Given a strictly feasible interior point x_k > 0 satisfying A * x_k = b:
 *   1. Diagonal scaling matrix: D = diag(x_k).
 *   2. Dual estimate w is obtained by solving the m x m symmetric system:
 *        (A * D^2 * A^T) * w = A * D^2 * c
 *   3. Reduced cost vector:
 *        r = c - A^T * w
 *   4. Primal step direction:
 *        d_x = - D^2 * r
 *   5. Maximum allowable step size to boundary of positive orthant:
 *        theta = min_{i: (d_x)_i < 0} ( - (x_k)_i / (d_x)_i )
 *      Step length alpha = gamma * theta (with safety damping gamma in (0, 1), e.g. 0.95).
 *   6. Update iterate:
 *        x_{k+1} = x_k + alpha * d_x.
 *
 * Complexity:
 *   - Each iteration solves an m x m linear system: O(m^3 + m^2 * n).
 *   - Practical convergence in few tens of iterations.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <algorithm>

namespace optimization {

using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;

/**
 * @brief Solves M * x = rhs using Gaussian Elimination with partial pivoting.
 */
bool solveLinearSystem(Matrix M, Vector rhs, Vector& sol) {
    size_t n = M.size();
    sol.assign(n, 0.0);

    for (size_t col = 0; col < n; ++col) {
        size_t pivot = col;
        double maxVal = std::abs(M[col][col]);
        for (size_t row = col + 1; row < n; ++row) {
            if (std::abs(M[row][col]) > maxVal) {
                maxVal = std::abs(M[row][col]);
                pivot = row;
            }
        }
        if (maxVal < 1e-12) return false;

        std::swap(M[col], M[pivot]);
        std::swap(rhs[col], rhs[pivot]);

        for (size_t row = col + 1; row < n; ++row) {
            double factor = M[row][col] / M[col][col];
            for (size_t j = col; j < n; ++j) {
                M[row][j] -= factor * M[col][j];
            }
            rhs[row] -= factor * rhs[col];
        }
    }

    for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
        double sum = rhs[i];
        for (size_t j = static_cast<size_t>(i) + 1; j < n; ++j) {
            sum -= M[i][j] * sol[j];
        }
        sol[i] = sum / M[i][i];
    }
    return true;
}

struct AffineResult {
    bool optimal;
    Vector x;
    double objectiveValue;
    int iterations;
};

/**
 * @brief Dikin's Primal Affine Scaling Solver.
 * @param c Cost vector.
 * @param A Equality constraint matrix (m x n).
 * @param b Equality RHS vector (m).
 * @param x0 Strictly interior initial feasible point (x0 > 0, A * x0 = b).
 * @param gamma Step damping factor (e.g. 0.95).
 * @param tol Convergence tolerance.
 * @return AffineResult containing final solution and objective value.
 */
AffineResult solveAffineScaling(const Vector& c, const Matrix& A, const Vector& b,
                                Vector x0, double gamma = 0.95, double tol = 1e-7, int maxIter = 100) {
    (void)b; // b is satisfied by feasibility of x0
    size_t m = A.size();
    size_t n = c.size();
    Vector x = x0;

    int iter = 0;
    for (; iter < maxIter; ++iter) {
        // Compute D^2 * c and D^2 * A^T
        // D_i = x_i -> D^2_i = x_i^2
        Vector D2_c(n);
        for (size_t i = 0; i < n; ++i) {
            D2_c[i] = x[i] * x[i] * c[i];
        }

        // Form M = A * D^2 * A^T (m x m)
        Matrix M(m, Vector(m, 0.0));
        for (size_t i = 0; i < m; ++i) {
            for (size_t j = 0; j < m; ++j) {
                double sum = 0.0;
                for (size_t k = 0; k < n; ++k) {
                    sum += A[i][k] * x[k] * x[k] * A[j][k];
                }
                M[i][j] = sum;
            }
        }

        // Form rhs = A * D^2 * c (m)
        Vector rhs(m, 0.0);
        for (size_t i = 0; i < m; ++i) {
            for (size_t k = 0; k < n; ++k) {
                rhs[i] += A[i][k] * D2_c[k];
            }
        }

        // Solve M * w = rhs for dual variables w
        Vector w;
        if (!solveLinearSystem(M, rhs, w)) {
            break;
        }

        // Reduced costs: r = c - A^T * w
        Vector r(n);
        for (size_t k = 0; k < n; ++k) {
            double at_w = 0.0;
            for (size_t i = 0; i < m; ++i) {
                at_w += A[i][k] * w[i];
            }
            r[k] = c[k] - at_w;
        }

        // Direction d_x = - D^2 * r
        Vector dx(n);
        double maxDxNorm = 0.0;
        for (size_t k = 0; k < n; ++k) {
            dx[k] = -x[k] * x[k] * r[k];
            maxDxNorm = std::max(maxDxNorm, std::abs(dx[k]));
        }

        // Check optimality / convergence
        if (maxDxNorm < tol) {
            break;
        }

        // Ratio test: find maximum step size alpha to keep x > 0
        double minRatio = 1e20;
        bool hasNegative = false;
        for (size_t k = 0; k < n; ++k) {
            if (dx[k] < -1e-12) {
                double ratio = -x[k] / dx[k];
                if (ratio < minRatio) {
                    minRatio = ratio;
                    hasNegative = true;
                }
            }
        }

        if (!hasNegative) {
            // Problem is unbounded below
            return {false, x, -1e20, iter};
        }

        double alpha = gamma * minRatio;

        // Update iterate: x = x + alpha * dx
        double change = 0.0;
        for (size_t k = 0; k < n; ++k) {
            x[k] += alpha * dx[k];
            change += std::abs(alpha * dx[k]);
        }

        if (change < tol) {
            break;
        }
    }

    double obj = 0.0;
    for (size_t k = 0; k < n; ++k) {
        obj += c[k] * x[k];
    }

    return {true, x, obj, iter + 1};
}

} // namespace optimization

int main() {
    std::cout << "========================================\n";
    std::cout << "  AFFINE SCALING INTERIOR POINT METHOD  \n";
    std::cout << "========================================\n\n";

    // Standard Form Linear Program:
    // Minimize   -2 * x_1 - x_2
    // Subject to:
    //    x_1 + x_2 + s_1       = 4
    //    x_1       +     s_2   = 2
    //    x_1, x_2, s_1, s_2 >= 0
    //
    // True optimal vertex: x_1 = 2, x_2 = 2, s_1 = 0, s_2 = 0
    // Objective value: -2(2) - 1(2) = -6.

    optimization::Vector c = {-2.0, -1.0, 0.0, 0.0};
    optimization::Matrix A = {
        {1.0, 1.0, 1.0, 0.0},
        {1.0, 0.0, 0.0, 1.0}
    };
    optimization::Vector b = {4.0, 2.0};

    // Strictly positive feasible interior starting point:
    // Let x_1 = 1.0, x_2 = 1.0 -> s_1 = 4 - (1+1) = 2.0, s_2 = 2 - 1 = 1.0
    // x_0 = [1.0, 1.0, 2.0, 1.0] > 0
    optimization::Vector x0 = {1.0, 1.0, 2.0, 1.0};

    std::cout << "Problem formulation (in standard equality form):\n";
    std::cout << "  Minimize  -2*x1 - x2\n";
    std::cout << "  Subject to:\n";
    std::cout << "    x1 + x2 + s1 = 4\n";
    std::cout << "    x1      + s2 = 2\n";
    std::cout << "    x1, x2, s1, s2 >= 0\n";
    std::cout << "  Initial interior point x0 = [1.0, 1.0, 2.0, 1.0]\n\n";

    auto res = optimization::solveAffineScaling(c, A, b, x0);

    std::cout << "Optimization Results:\n";
    std::cout << "  Iterations  : " << res.iterations << "\n";
    std::cout << "  Converged?  : " << (res.optimal ? "YES" : "NO") << "\n";
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "  Optimal x1  : " << res.x[0] << " (Theoretical: 2.00000)\n";
    std::cout << "  Optimal x2  : " << res.x[1] << " (Theoretical: 2.00000)\n";
    std::cout << "  Slack s1    : " << res.x[2] << " (Theoretical: 0.00000)\n";
    std::cout << "  Slack s2    : " << res.x[3] << " (Theoretical: 0.00000)\n";
    std::cout << "  Min Value   : " << res.objectiveValue << " (Theoretical: -6.00000)\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
