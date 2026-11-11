/**
 * @file nelder_mead_simplex_optimization.cpp
 * @brief Nelder-Mead Downhill Simplex method for derivative-free nonlinear optimization in C++17.
 *
 * Overview (Nelder & Mead, 1965):
 *   A heuristic search method that finds a local minimum of an objective function f(x)
 *   in N-dimensional space without requiring gradients or Hessians.
 *   It maintains a simplex of (N + 1) vertices and iteratively adapts its shape via:
 *     1. Reflection  (alpha = 1.0)
 *     2. Expansion   (gamma = 2.0)
 *     3. Contraction (rho   = 0.5)
 *     4. Shrink      (sigma = 0.5)
 *
 * Convergence Criteria:
 *   Terminates when standard deviation of objective values across the simplex vertices
 *   and/or simplex diameter falls below tolerance, or max iterations is reached.
 *
 * Complexity:
 *   - Time per iteration: O(N log N) for sorting (N+1) vertices + O(N) vector operations.
 *   - Memory: O(N^2) to store the (N+1) simplex points of dimension N.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <numeric>

namespace optimization {

using Vector = std::vector<double>;

struct NelderMeadResult {
    Vector bestPoint;
    double bestValue;
    int iterations;
    bool converged;
};

/**
 * @brief Runs Nelder-Mead optimization on an N-dimensional objective function.
 */
NelderMeadResult nelderMead(const std::function<double(const Vector&)>& func,
                            const Vector& initialGuess,
                            double stepSize = 0.5,
                            double tol = 1e-8,
                            int maxIter = 1000) {
    const double alpha = 1.0; // Reflection
    const double gamma = 2.0; // Expansion
    const double rho = 0.5;   // Contraction
    const double sigma = 0.5; // Shrink

    size_t n = initialGuess.size();
    struct Vertex {
        Vector x;
        double f;
    };

    // Initialize (N + 1) simplex vertices
    std::vector<Vertex> simplex(n + 1);
    simplex[0] = {initialGuess, func(initialGuess)};
    for (size_t i = 0; i < n; ++i) {
        Vector pt = initialGuess;
        pt[i] += stepSize;
        simplex[i + 1] = {pt, func(pt)};
    }

    auto vecAddScaled = [&](const Vector& a, const Vector& b, double scale) {
        Vector res(n);
        for (size_t i = 0; i < n; ++i) res[i] = a[i] + scale * (b[i] - a[i]);
        return res;
    };

    int iter = 0;
    for (; iter < maxIter; ++iter) {
        // 1. Order vertices: f[0] <= f[1] <= ... <= f[n]
        std::sort(simplex.begin(), simplex.end(),
                  [](const Vertex& a, const Vertex& b) { return a.f < b.f; });

        // Check convergence: standard deviation of objective values
        double meanF = 0.0;
        for (const auto& v : simplex) meanF += v.f;
        meanF /= static_cast<double>(n + 1);

        double variance = 0.0;
        for (const auto& v : simplex) variance += (v.f - meanF) * (v.f - meanF);
        double stdDev = std::sqrt(variance / static_cast<double>(n + 1));

        if (stdDev < tol) {
            return {simplex[0].x, simplex[0].f, iter, true};
        }

        // 2. Compute centroid of best N vertices (excluding worst vertex n)
        Vector centroid(n, 0.0);
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                centroid[j] += simplex[i].x[j];
            }
        }
        for (size_t j = 0; j < n; ++j) {
            centroid[j] /= static_cast<double>(n);
        }

        // 3. Reflection: x_r = x_o + alpha * (x_o - x_n) = x_n + (1 + alpha) * (x_o - x_n)
        Vector xr(n);
        for (size_t j = 0; j < n; ++j) {
            xr[j] = centroid[j] + alpha * (centroid[j] - simplex[n].x[j]);
        }
        double fr = func(xr);

        if (simplex[0].f <= fr && fr < simplex[n - 1].f) {
            simplex[n] = {xr, fr};
            continue;
        }

        // 4. Expansion
        if (fr < simplex[0].f) {
            Vector xe = vecAddScaled(centroid, xr, gamma);
            double fe = func(xe);
            if (fe < fr) {
                simplex[n] = {xe, fe};
            } else {
                simplex[n] = {xr, fr};
            }
            continue;
        }

        // 5. Contraction
        if (fr < simplex[n].f) {
            // Outside contraction
            Vector xc = vecAddScaled(centroid, xr, rho);
            double fc = func(xc);
            if (fc <= fr) {
                simplex[n] = {xc, fc};
                continue;
            }
        } else {
            // Inside contraction
            Vector xc = vecAddScaled(centroid, simplex[n].x, rho);
            double fc = func(xc);
            if (fc < simplex[n].f) {
                simplex[n] = {xc, fc};
                continue;
            }
        }

        // 6. Shrink towards best vertex x_0
        for (size_t i = 1; i <= n; ++i) {
            simplex[i].x = vecAddScaled(simplex[0].x, simplex[i].x, sigma);
            simplex[i].f = func(simplex[i].x);
        }
    }

    std::sort(simplex.begin(), simplex.end(),
              [](const Vertex& a, const Vertex& b) { return a.f < b.f; });
    return {simplex[0].x, simplex[0].f, iter, false};
}

} // namespace optimization

int main() {
    std::cout << "========================================\n";
    std::cout << "   NELDER-MEAD SIMPLEX OPTIMIZATION     \n";
    std::cout << "  (Derivative-Free Multidimensional)    \n";
    std::cout << "========================================\n\n";

    // Test 1: Rosenbrock's Banana Function: f(x, y) = (1 - x)^2 + 100*(y - x^2)^2
    // True global minimum: (1.0, 1.0), f(1, 1) = 0.0
    auto rosenbrock = [](const optimization::Vector& v) -> double {
        double x = v[0], y = v[1];
        return std::pow(1.0 - x, 2) + 100.0 * std::pow(y - x * x, 2);
    };

    std::cout << "Test 1: Rosenbrock Function f(x, y) = (1 - x)^2 + 100*(y - x^2)^2\n";
    std::cout << "  Start point: (-1.2, 1.0)\n";
    auto res1 = optimization::nelderMead(rosenbrock, {-1.2, 1.0}, 0.5, 1e-10, 1000);
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "  Converged in : " << res1.iterations << " iterations\n";
    std::cout << "  Found Min    : (" << res1.bestPoint[0] << ", " << res1.bestPoint[1] << ")\n";
    std::cout << "  Min Value    : " << std::scientific << res1.bestValue << std::fixed << "\n\n";

    // Test 2: Himmelblau's Function: f(x, y) = (x^2 + y - 11)^2 + (x + y^2 - 7)^2
    // Known exact minimum near (3.0, 2.0) with f(3, 2) = 0.0
    auto himmelblau = [](const optimization::Vector& v) -> double {
        double x = v[0], y = v[1];
        return std::pow(x * x + y - 11.0, 2) + std::pow(x + y * y - 7.0, 2);
    };

    std::cout << "Test 2: Himmelblau's Function f(x, y) = (x^2 + y - 11)^2 + (x + y^2 - 7)^2\n";
    std::cout << "  Start point: (0.0, 0.0)\n";
    auto res2 = optimization::nelderMead(himmelblau, {0.0, 0.0}, 1.0, 1e-10, 1000);
    std::cout << "  Converged in : " << res2.iterations << " iterations\n";
    std::cout << "  Found Min    : (" << res2.bestPoint[0] << ", " << res2.bestPoint[1] << ")\n";
    std::cout << "  Min Value    : " << std::scientific << res2.bestValue << std::fixed << "\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
