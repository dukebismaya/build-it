/**
 * @file interior_point_linear_programming.cpp
 * @brief Primal Logarithmic Barrier Path-Following Interior Point Method for LP in C++17.
 *
 * Linear Programming Problem Formulation:
 *   Minimize   c^T * x
 *   Subject to A * x <= b
 *
 * Logarithmic Barrier Function:
 *   Introduce slack s_i = b_i - a_i^T * x > 0.
 *   The unconstrained penalized objective for barrier parameter mu > 0 is:
 *     phi_mu(x) = c^T * x - mu * sum_{i=1}^m ln(b_i - a_i^T * x)
 *
 * First and Second Derivatives:
 *   Let d_i = 1 / (b_i - a_i^T * x)
 *   Gradient:  grad = c + mu * A^T * d
 *   Hessian:   H = mu * A^T * diag(d^2) * A
 *
 * Newton Step:
 *   Solve H * delta_x = -grad
 *   Step length alpha in (0, 1] found by line search ensuring A * (x + alpha * delta_x) < b.
 *   Reduce mu (e.g. mu := mu * gamma, gamma in [0.2, 0.5]) until mu * m < tol.
 *
 * Complexity:
 *   - Newton step calculation (solving n x n linear system): O(n^3 + m * n^2).
 *   - Overall convergence: O(sqrt(m) * log(1 / epsilon)) iterations.
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
 * @brief Solves linear system M * x = rhs using Gaussian Elimination with partial pivoting.
 */
bool solveLinearSystem(Matrix M, Vector rhs, Vector& sol) {
    size_t n = M.size();
    sol.assign(n, 0.0);

    for (size_t col = 0; col < n; ++col) {
        // Pivot selection
        size_t pivot = col;
        double maxVal = std::abs(M[col][col]);
        for (size_t row = col + 1; row < n; ++row) {
            if (std::abs(M[row][col]) > maxVal) {
                maxVal = std::abs(M[row][col]);
                pivot = row;
            }
        }
        if (maxVal < 1e-12) return false; // Singular matrix

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

    // Back substitution
    for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
        double sum = rhs[i];
        for (size_t j = static_cast<size_t>(i) + 1; j < n; ++j) {
            sum -= M[i][j] * sol[j];
        }
        sol[i] = sum / M[i][i];
    }
    return true;
}

struct LPResult {
    bool optimal;
    Vector x;
    double objectiveValue;
    int outerIterations;
};

/**
 * @brief Primal Logarithmic Barrier Interior Point Solver.
 * @param c Cost vector (minimize c^T * x).
 * @param A Constraint matrix (A * x <= b).
 * @param b Constraint bound vector.
 * @param x0 Strictly feasible initial interior point (A * x0 < b).
 * @param tol Termination tolerance.
 * @return LPResult containing optimal solution vector and cost.
 */
LPResult solveBarrierLP(const Vector& c, const Matrix& A, const Vector& b,
                        Vector x0, double tol = 1e-6) {
    size_t n = c.size();
    size_t m = b.size();

    Vector x = x0;
    double mu = 1.0;
    const double gamma = 0.5; // Barrier reduction rate
    int outerIter = 0;

    while (mu * static_cast<double>(m) > tol && outerIter < 100) {
        outerIter++;

        // Inner Newton centering iterations
        for (int inner = 0; inner < 20; ++inner) {
            // Compute slacks s_i = b_i - a_i^T * x
            Vector s(m), d(m);
            for (size_t i = 0; i < m; ++i) {
                double ax = 0.0;
                for (size_t j = 0; j < n; ++j) {
                    ax += A[i][j] * x[j];
                }
                s[i] = b[i] - ax;
                if (s[i] <= 0.0) s[i] = 1e-12; // Numerical safety
                d[i] = 1.0 / s[i];
            }

            // Gradient: grad = c + mu * A^T * d
            Vector grad = c;
            for (size_t j = 0; j < n; ++j) {
                double at_d = 0.0;
                for (size_t i = 0; i < m; ++i) {
                    at_d += A[i][j] * d[i];
                }
                grad[j] += mu * at_d;
            }

            // Check inner convergence (norm of gradient)
            double gradNorm = 0.0;
            for (double g : grad) gradNorm += g * g;
            if (std::sqrt(gradNorm) < 1e-4) break;

            // Hessian: H = mu * A^T * diag(d^2) * A
            Matrix H(n, Vector(n, 0.0));
            for (size_t i = 0; i < m; ++i) {
                double d2 = d[i] * d[i];
                for (size_t j = 0; j < n; ++j) {
                    for (size_t k = 0; k < n; ++k) {
                        H[j][k] += mu * A[i][j] * A[i][k] * d2;
                    }
                }
            }

            // Newton step: H * delta_x = -grad
            Vector negGrad(n);
            for (size_t j = 0; j < n; ++j) negGrad[j] = -grad[j];

            Vector deltaX;
            if (!solveLinearSystem(H, negGrad, deltaX)) {
                break;
            }

            // Backtracking line search for step length alpha
            double alpha = 1.0;
            while (alpha > 1e-8) {
                bool feasible = true;
                for (size_t i = 0; i < m; ++i) {
                    double ax_new = 0.0;
                    for (size_t j = 0; j < n; ++j) {
                        ax_new += A[i][j] * (x[j] + alpha * deltaX[j]);
                    }
                    if (b[i] - ax_new <= 0.0) {
                        feasible = false;
                        break;
                    }
                }
                if (feasible) break;
                alpha *= 0.5;
            }

            for (size_t j = 0; j < n; ++j) {
                x[j] += alpha * deltaX[j];
            }
        }

        mu *= gamma;
    }

    double obj = 0.0;
    for (size_t j = 0; j < n; ++j) {
        obj += c[j] * x[j];
    }

    return {true, x, obj, outerIter};
}

} // namespace optimization

int main() {
    std::cout << "========================================\n";
    std::cout << "  INTERIOR POINT METHOD (LOG BARRIER)   \n";
    std::cout << "========================================\n\n";

    // Linear Program:
    // Maximize  3 * x_1 + 2 * x_2
    // Minimize -3 * x_1 - 2 * x_2
    // Subject to:
    //   x_1 + x_2 <= 4
    //   x_1 <= 2
    //  -x_1 <= 0  (x_1 >= 0)
    //  -x_2 <= 0  (x_2 >= 0)
    //
    // True optimal vertex: (x_1 = 2, x_2 = 2) -> Max value = 3(2) + 2(2) = 10, Min obj = -10.

    optimization::Vector c = {-3.0, -2.0};
    optimization::Matrix A = {
        { 1.0,  1.0},
        { 1.0,  0.0},
        {-1.0,  0.0},
        { 0.0, -1.0}
    };
    optimization::Vector b = {4.0, 2.0, 0.0, 0.0};

    // Strictly interior starting point: x_0 = (0.5, 0.5)
    // Checks: 0.5 + 0.5 = 1 < 4; 0.5 < 2; -0.5 < 0; -0.5 < 0 (All strict inequalities hold)
    optimization::Vector x0 = {0.5, 0.5};

    std::cout << "Problem setup:\n";
    std::cout << "  Minimize  -3 * x_1 - 2 * x_2\n";
    std::cout << "  Subject to:\n";
    std::cout << "    x_1 + x_2 <= 4\n";
    std::cout << "    x_1 <= 2\n";
    std::cout << "    x_1 >= 0, x_2 >= 0\n\n";

    auto result = optimization::solveBarrierLP(c, A, b, x0);

    std::cout << "Interior Point Optimization Result:\n";
    std::cout << "  Converged In : " << result.outerIterations << " barrier reduction iterations\n";
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "  Optimal x_1  : " << result.x[0] << " (Theoretical: 2.00000)\n";
    std::cout << "  Optimal x_2  : " << result.x[1] << " (Theoretical: 2.00000)\n";
    std::cout << "  Min Objective: " << result.objectiveValue << " (Max Profit: " << -result.objectiveValue << ")\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
