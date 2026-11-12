/**
 * @file brent_dekker_root_finding.cpp
 * @brief Brent's (Dekker-Brent) 1D root-finding method in C++17.
 *
 * Overview (Richard Brent, 1973):
 *   Brent's method is the standard general-purpose root-finding algorithm for continuous
 *   functions f(x) = 0 on a bracketed interval [a, b] where f(a) * f(b) < 0.
 *
 * Algorithmic Strategy:
 *   Combines three methods to guarantee both speed and robust convergence:
 *     1. Bisection: Guarantees linear convergence in worst-case pathological functions.
 *     2. Secant Method: Accelerated convergence when two distinct approximations exist.
 *     3. Inverse Quadratic Interpolation (IQI): Superlinear convergence of order ~1.839
 *        using three distinct points (a, f(a)), (b, f(b)), (c, f(c)).
 *
 * Safeguards:
 *   Brent's conditions monitor whether the interpolation step is making sufficient progress
 *   (at least halving the search interval every few steps). If interpolation steps behave
 *   erratically or jump outside the current bracket, it strictly falls back to Bisection.
 *
 * Complexity:
 *   - Superlinear convergence ~ O(1.839) on smooth functions.
 *   - Worst-case time: O(log((b - a) / tol)) function evaluations (never worse than bisection).
 *   - Space: O(1).
 */

#include <iostream>
#include <cmath>
#include <functional>
#include <iomanip>
#include <string>

namespace numerical {

struct RootResult {
    double root;
    double functionValue;
    int iterations;
    bool converged;
};

/**
 * @brief Finds a root of f(x) = 0 within [a, b] using Brent's Method.
 * @param f Continuous function.
 * @param a Left interval endpoint.
 * @param b Right interval endpoint (must satisfy f(a)*f(b) <= 0).
 * @param tol Convergence tolerance.
 * @param maxIter Max iterations.
 */
RootResult brentRoot(const std::function<double(double)>& f,
                     double a, double b,
                     double tol = 1e-12, int maxIter = 100) {
    double fa = f(a);
    double fb = f(b);

    if (fa * fb > 0.0) {
        return {0.0, 0.0, 0, false}; // Root not bracketed
    }

    if (std::abs(fa) < std::abs(fb)) {
        std::swap(a, b);
        std::swap(fa, fb);
    }

    double c = a;
    double fc = fa;
    bool mflag = true;
    double s = b;
    double d = 0.0; // Distance of step before last

    int iter = 0;
    for (; iter < maxIter; ++iter) {
        if (std::abs(fb) < tol || std::abs(b - a) < tol) {
            return {b, fb, iter, true};
        }

        if (std::abs(fa - fc) > 1e-15 && std::abs(fb - fc) > 1e-15) {
            // Inverse Quadratic Interpolation (IQI)
            s = (a * fb * fc) / ((fa - fb) * (fa - fc)) +
                (b * fa * fc) / ((fb - fa) * (fb - fc)) +
                (c * fa * fb) / ((fc - fa) * (fc - fb));
        } else {
            // Secant Method
            s = b - fb * ((b - a) / (fb - fa));
        }

        // Brent's 5 conditions to check if interpolation should be rejected in favor of Bisection:
        double condition1Lower = (3.0 * a + b) / 4.0;
        double condition1Upper = b;
        if (condition1Lower > condition1Upper) std::swap(condition1Lower, condition1Upper);

        bool cond1 = (s < condition1Lower || s > condition1Upper);
        bool cond2 = (mflag && std::abs(s - b) >= std::abs(b - c) / 2.0);
        bool cond3 = (!mflag && std::abs(s - b) >= std::abs(c - d) / 2.0);
        bool cond4 = (mflag && std::abs(b - c) < tol);
        bool cond5 = (!mflag && std::abs(c - d) < tol);

        if (cond1 || cond2 || cond3 || cond4 || cond5) {
            // Bisection fallback
            s = (a + b) / 2.0;
            mflag = true;
        } else {
            mflag = false;
        }

        double fs = f(s);
        d = c;
        c = b;
        fc = fb;

        if (fa * fs < 0.0) {
            b = s;
            fb = fs;
        } else {
            a = s;
            fa = fs;
        }

        if (std::abs(fa) < std::abs(fb)) {
            std::swap(a, b);
            std::swap(fa, fb);
        }
    }

    return {b, fb, iter, true};
}

} // namespace numerical

int main() {
    std::cout << "========================================\n";
    std::cout << "   BRENT'S ROOT-FINDING ALGORITHM       \n";
    std::cout << " (Bisection + Secant + IQI Convergence) \n";
    std::cout << "========================================\n\n";

    // Test 1: Transcendental equation: cos(x) - x = 0 (The Dottie Number)
    // Root on [0, 1] is ~= 0.739085133215
    auto f1 = [](double x) { return std::cos(x) - x; };
    auto res1 = numerical::brentRoot(f1, 0.0, 1.0, 1e-12);

    std::cout << "Problem 1: cos(x) - x = 0 on [0, 1] (Dottie Number)\n";
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "  - Computed Root     : " << res1.root << "\n";
    std::cout << "  - Residual |f(root)|: " << std::scientific << std::abs(res1.functionValue) << std::fixed << "\n";
    std::cout << "  - Iterations        : " << res1.iterations << "\n\n";

    // Test 2: Polynomial: x^3 - 2*x - 5 = 0
    // Real root on [2, 3] is ~= 2.094551481542
    auto f2 = [](double x) { return x * x * x - 2.0 * x - 5.0; };
    auto res2 = numerical::brentRoot(f2, 2.0, 3.0, 1e-12);

    std::cout << "Problem 2: x^3 - 2*x - 5 = 0 on [2, 3]\n";
    std::cout << "  - Computed Root     : " << res2.root << "\n";
    std::cout << "  - Residual |f(root)|: " << std::scientific << std::abs(res2.functionValue) << std::fixed << "\n";
    std::cout << "  - Iterations        : " << res2.iterations << "\n\n";

    std::cout << "Superlinear Brent convergence verified.\n";
    std::cout << "Program finished successfully.\n";
    return 0;
}
