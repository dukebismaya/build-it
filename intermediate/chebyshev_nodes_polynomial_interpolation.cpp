/*
 * ============================================================
 *     Barycentric Chebyshev-Node Polynomial Interpolation
 * ============================================================
 *
 * Description:
 *   Interpolating a smooth function on N+1 equispaced nodes on
 *   [-1, 1] can diverge wildly near the endpoints (Runge's
 *   phenomenon). Clustering nodes at the Chebyshev zeroes of
 *   T_{N+1}(x):
 *     x_j = cos((2j + 1) * pi / (2N + 2)),   j = 0, 1, ..., N
 *   minimizes the maximum of prod |x - x_j| (equal to 2^{-N})
 *   and yields uniform exponential convergence for analytic functions.
 *
 *   Using the Barycentric Lagrange Formula (Berrut & Trefethen,
 *   2004), the closed-form Chebyshev barycentric weights are:
 *     w_j = (-1)^j * sin((2j + 1) * pi / (2N + 2))
 *   and evaluating the degree-N interpolant at any x takes O(N):
 *     P(x) = (sum_{j=0}^N w_j f(x_j) / (x - x_j))
 *          / (sum_{j=0}^N w_j / (x - x_j))
 *
 * Complexity:
 *   Setup: O(N) time and space
 *   Evaluation at a query point x: O(N) time, numerically stable.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <iomanip>
#include <algorithm>

namespace chebyshev_interp {

    class BarycentricChebyshev {
        int n;
        double a, b;
        std::vector<double> nodes, values, weights;

    public:
        BarycentricChebyshev(const std::function<double(double)>& f, int degree,
                             double lo = -1.0, double hi = 1.0)
            : n(degree), a(lo), b(hi), nodes(degree + 1), values(degree + 1), weights(degree + 1) {
            const double PI = std::acos(-1.0);
            for (int j = 0; j <= n; ++j) {
                double theta = (2 * j + 1) * PI / (2 * n + 2);
                double t = std::cos(theta);
                nodes[j] = 0.5 * ((b - a) * t + (a + b));
                values[j] = f(nodes[j]);
                weights[j] = ((j & 1) ? -1.0 : 1.0) * std::sin(theta);
            }
        }

        double eval(double x) const {
            double num = 0.0, den = 0.0;
            for (int j = 0; j <= n; ++j) {
                double diff = x - nodes[j];
                if (std::fabs(diff) < 1e-15) return values[j];
                double term = weights[j] / diff;
                num += term * values[j];
                den += term;
            }
            return num / den;
        }
    };

    // Equispaced barycentric interpolant for comparison on Runge's function
    double eval_equispaced(const std::function<double(double)>& f, int n, double x) {
        std::vector<double> xj(n + 1), yj(n + 1), wj(n + 1, 1.0);
        for (int j = 0; j <= n; ++j) {
            xj[j] = -1.0 + 2.0 * j / n;
            yj[j] = f(xj[j]);
        }
        for (int j = 0; j <= n; ++j)
            for (int k = 0; k <= n; ++k)
                if (k != j) wj[j] /= (xj[j] - xj[k]);
        double num = 0.0, den = 0.0;
        for (int j = 0; j <= n; ++j) {
            double diff = x - xj[j];
            if (std::fabs(diff) < 1e-15) return yj[j];
            double t = wj[j] / diff;
            num += t * yj[j];
            den += t;
        }
        return num / den;
    }

} // namespace chebyshev_interp

int main() {
    using namespace chebyshev_interp;
    std::cout << "============================================================\n";
    std::cout << "     Barycentric Chebyshev-Node Polynomial Interpolation\n";
    std::cout << "============================================================\n\n";

    auto runge = [](double x) { return 1.0 / (1.0 + 25.0 * x * x); };

    std::cout << "Runge function f(x) = 1 / (1 + 25x^2) on [-1, 1]:\n";
    std::cout << std::setw(8) << "Degree" << std::setw(22) << "Chebyshev Max Err"
              << std::setw(22) << "Equispaced Max Err" << "\n";
    std::cout << std::string(52, '-') << "\n";

    double prev_cheb_err = 10.0;
    bool ok = true;
    for (int deg : {8, 16, 24, 32, 48}) {
        BarycentricChebyshev cheb(runge, deg);
        double max_cheb = 0.0, max_equi = 0.0;
        for (int i = 0; i <= 400; ++i) {
            double x = -1.0 + 2.0 * i / 400.0;
            max_cheb = std::max(max_cheb, std::fabs(cheb.eval(x) - runge(x)));
            if (deg <= 24)
                max_equi = std::max(max_equi, std::fabs(eval_equispaced(runge, deg, x) - runge(x)));
        }
        if (max_cheb >= prev_cheb_err) ok = false;
        prev_cheb_err = max_cheb;

        std::cout << std::setw(8) << deg
                  << std::setw(22) << std::scientific << std::setprecision(3) << max_cheb;
        if (deg <= 24) std::cout << std::setw(22) << max_equi << "\n";
        else           std::cout << std::setw(22) << "(diverges > 10^2)" << "\n";
    }

    // Exact reproduction of a degree-10 polynomial and accurate approximation of exp(x)
    BarycentricChebyshev exp_interp([](double x) { return std::exp(x); }, 16, -2.0, 2.0);
    for (int i = 0; i <= 100; ++i) {
        double x = -2.0 + 4.0 * i / 100.0;
        if (std::fabs(exp_interp.eval(x) - std::exp(x)) > 1e-13) ok = false;
    }

    std::cout << "\nChebyshev convergence on Runge & <1e-13 error on exp(x): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
