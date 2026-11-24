/*
 * ============================================================
 *                Golden-Section Search
 * ============================================================
 *
 * Description:
 *   Golden-section search finds the minimum of a unimodal
 *   function on [a, b] without derivatives. Like ternary search
 *   it shrinks the bracket each step, but it places the probe
 *   points at the golden ratio so that ONE of the two interior
 *   points is reused in the next iteration:
 *
 *     invphi = (sqrt(5) - 1) / 2  ~ 0.618
 *     c = b - invphi * (b - a)
 *     d = a + invphi * (b - a)
 *     if f(c) < f(d): b = d, d = c, c = b - invphi (b - a)
 *     else           a = c, c = d, d = a + invphi (b - a)
 *
 *   Each iteration costs a single function evaluation and
 *   shrinks the interval by 0.618 (ternary search: 2 evals for
 *   a factor 0.667, i.e. ~0.816 per evaluation).
 *
 * Complexity:
 *   O(log(1/eps)) evaluations, ~1.44 log2((b-a)/eps).
 * ============================================================
 */

#include <iostream>
#include <cmath>
#include <functional>
#include <iomanip>
#include <string>

namespace golden {

    struct Result { double x; int evals; };

    Result minimize(const std::function<double(double)>& f, double a, double b, double tol = 1e-10) {
        const double invphi = (std::sqrt(5.0) - 1.0) / 2.0;
        double c = b - invphi * (b - a), d = a + invphi * (b - a);
        double fc = f(c), fd = f(d);
        int evals = 2;
        while (b - a > tol) {
            if (fc < fd) {
                b = d; d = c; fd = fc;
                c = b - invphi * (b - a); fc = f(c);
            } else {
                a = c; c = d; fc = fd;
                d = a + invphi * (b - a); fd = f(d);
            }
            ++evals;
        }
        return {(a + b) / 2, evals};
    }

    Result ternary(const std::function<double(double)>& f, double a, double b, double tol = 1e-10) {
        int evals = 0;
        while (b - a > tol) {
            double m1 = a + (b - a) / 3, m2 = b - (b - a) / 3;
            if (f(m1) < f(m2)) b = m2; else a = m1;
            evals += 2;
        }
        return {(a + b) / 2, evals};
    }

} // namespace golden

int main() {
    std::cout << "============================================================\n";
    std::cout << "                  Golden-Section Search\n";
    std::cout << "============================================================\n\n";

    struct Test {
        std::string name;
        std::function<double(double)> f;
        double a, b, expected;
    };
    const double PI = std::acos(-1.0);
    Test tests[] = {
        {"(x - 2)^2 + 1",       [](double x) { return (x - 2) * (x - 2) + 1; }, -10, 10, 2.0},
        {"x^4 - 3x^3 + 2",      [](double x) { return x * x * x * x - 3 * x * x * x + 2; }, 0, 5, 2.25},
        {"-sin(x) on [0, pi]",  [](double x) { return -std::sin(x); }, 0, PI, PI / 2},
        {"e^x - 4x",            [](double x) { return std::exp(x) - 4 * x; }, 0, 3, std::log(4.0)},
        {"|x - 0.3| (non-smooth)", [](double x) { return std::fabs(x - 0.3); }, -1, 1, 0.3},
    };

    bool ok = true;
    std::cout << std::left << std::setw(26) << "Function" << std::setw(18) << "Golden x*"
              << std::setw(8) << "evals" << std::setw(18) << "Ternary x*" << "evals\n";
    std::cout << std::string(76, '-') << "\n";
    for (auto& t : tests) {
        auto g = golden::minimize(t.f, t.a, t.b);
        auto r = golden::ternary(t.f, t.a, t.b);
        if (std::fabs(g.x - t.expected) > 1e-7) ok = false;
        std::cout << std::setw(26) << t.name << std::setw(18) << std::setprecision(10) << g.x
                  << std::setw(8) << g.evals << std::setw(18) << r.x << r.evals << "\n";
    }
    std::cout << "\nAll minima within 1e-7 of the analytic answer: " << (ok ? "PASSED" : "FAILED") << "\n";
    std::cout << "Golden-section needs ~30% fewer evaluations than ternary search.\n";
    return 0;
}
