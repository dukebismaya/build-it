/*
 * ============================================================
 *               Romberg Numerical Integration
 * ============================================================
 *
 * Description:
 *   Romberg's method (Werner Romberg, 1955) combines the
 *   composite trapezoidal rule with Richardson extrapolation to
 *   accelerate convergence of definite integrals:
 *     I = integral_a^b f(x) dx
 *
 *   Table construction R(n, m) for 0 <= m <= n:
 *     - R(0, 0) = (b - a) / 2 * (f(a) + f(b))
 *     - R(n, 0) = 1/2 * R(n-1, 0) + h_n * sum_{k=1}^{2^{n-1}} f(a + (2k-1) h_n)
 *       where h_n = (b - a) / 2^n  (reuses previous evaluations!)
 *     - R(n, m) = R(n, m-1) + (R(n, m-1) - R(n-1, m-1)) / (4^m - 1)
 *
 *   Because the Euler-Maclaurin error expansion of the
 *   trapezoidal rule contains only even powers of h, each column
 *   m eliminates the O(h^{2m}) error term, achieving O(h_n^{2n+2})
 *   along the diagonal R(n, n).
 *
 * Complexity:
 *   2^N + 1 function evaluations and O(N^2) extrapolation work.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <iomanip>
#include <string>

namespace romberg {

    struct Result {
        double value;
        int levels;
        int evals;
        std::vector<std::vector<double>> table;
    };

    Result integrate(const std::function<double(double)>& f, double a, double b,
                     int max_levels = 10, double tol = 1e-13) {
        std::vector<std::vector<double>> R(max_levels);
        R[0].push_back(0.5 * (b - a) * (f(a) + f(b)));
        int evals = 2;

        for (int n = 1; n < max_levels; ++n) {
            R[n].resize(n + 1);
            double h = (b - a) / (1 << n);
            double sum = 0.0;
            int pts = 1 << (n - 1);
            for (int k = 1; k <= pts; ++k) sum += f(a + (2 * k - 1) * h);
            evals += pts;

            R[n][0] = 0.5 * R[n - 1][0] + h * sum;
            double p4 = 1.0;
            for (int m = 1; m <= n; ++m) {
                p4 *= 4.0;
                R[n][m] = R[n][m - 1] + (R[n][m - 1] - R[n - 1][m - 1]) / (p4 - 1.0);
            }
            if (n >= 2 && std::fabs(R[n][n] - R[n - 1][n - 1]) < tol)
                return {R[n][n], n + 1, evals, R};
        }
        int last = max_levels - 1;
        return {R[last][last], max_levels, evals, R};
    }

} // namespace romberg

int main() {
    using namespace romberg;
    const double PI = std::acos(-1.0);
    std::cout << "============================================================\n";
    std::cout << "               Romberg Numerical Integration\n";
    std::cout << "============================================================\n\n";

    std::cout << "Romberg triangular table for integral_0^pi sin(x) dx = 2:\n";
    auto r0 = integrate([](double x) { return std::sin(x); }, 0.0, PI, 6, 1e-15);
    for (int n = 0; n < r0.levels; ++n) {
        std::cout << "n=" << n << ": ";
        for (int m = 0; m <= n; ++m)
            std::cout << std::fixed << std::setprecision(12) << std::setw(15) << r0.table[n][m];
        std::cout << "\n";
    }

    struct Case {
        std::string name;
        std::function<double(double)> f;
        double a, b, exact;
    };
    Case cases[] = {
        {"int_0^pi sin(x) dx",        [](double x) { return std::sin(x); }, 0.0, PI, 2.0},
        {"int_0^1 4/(1+x^2) dx = pi", [](double x) { return 4.0 / (1.0 + x * x); }, 0.0, 1.0, PI},
        {"int_0^1 e^x dx = e - 1",    [](double x) { return std::exp(x); }, 0.0, 1.0, std::exp(1.0) - 1.0},
        {"int_1^e (1/x) dx = 1",      [](double x) { return 1.0 / x; }, 1.0, std::exp(1.0), 1.0},
        {"int_0^2 x^5 dx = 32/3",     [](double x) { return x * x * x * x * x; }, 0.0, 2.0, 32.0 / 3.0},
    };

    std::cout << "\n" << std::left << std::setw(28) << "Integral" << std::setw(20) << "Romberg"
              << std::setw(14) << "Abs Error" << "Evals\n" << std::string(70, '-') << "\n";
    bool ok = true;
    for (auto& c : cases) {
        auto res = integrate(c.f, c.a, c.b);
        double err = std::fabs(res.value - c.exact);
        if (err > 1e-12) ok = false;
        std::cout << std::setw(28) << c.name
                  << std::setw(20) << std::fixed << std::setprecision(15) << res.value
                  << std::setw(14) << std::scientific << std::setprecision(2) << err
                  << res.evals << "\n";
    }
    std::cout << "\nAll integrals accurate to < 1e-12: " << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
