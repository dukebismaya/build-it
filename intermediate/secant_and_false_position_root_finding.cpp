/*
 * ============================================================
 *        Secant and Illinois Regula Falsi Root Finding
 * ============================================================
 *
 * Description:
 *   Derivative-free root-finding algorithms for f(x) = 0:
 *
 *   1. Secant Method:
 *      Replaces f'(x_k) in Newton's method by a finite difference
 *      through the last two iterates:
 *        x_{k+1} = x_k - f(x_k) * (x_k - x_{k-1}) / (f(x_k) - f(x_{k-1}))
 *      Convergence order is the golden ratio phi ~ 1.618 (superlinear).
 *
 *   2. Regula Falsi (False Position) & Illinois Algorithm:
 *      Maintains a sign-change bracket [a, b] with f(a)f(b) < 0
 *      using the secant chord. Plain Regula Falsi can stagnate
 *      when one endpoint stays fixed on a convex function; the
 *      Illinois modification (Dowell & Jarratt, 1971) halves the
 *      function value of the retained endpoint whenever the same
 *      side is kept twice in a row, restoring superlinear
 *      convergence (order ~ 3^(1/3) ~ 1.442).
 *
 * Complexity:
 *   O(log log(1/eps)) function evaluations near a simple root.
 * ============================================================
 */

#include <iostream>
#include <cmath>
#include <functional>
#include <iomanip>
#include <string>

namespace root_finding {

    using Fn = std::function<double(double)>;
    struct Result { double root; int iters; bool ok; };

    Result secant(const Fn& f, double x0, double x1, double tol = 1e-13, int max_iter = 100) {
        double f0 = f(x0), f1 = f(x1);
        for (int k = 1; k <= max_iter; ++k) {
            if (std::fabs(f1 - f0) < 1e-18) return {x1, k, std::fabs(f1) < tol};
            double x2 = x1 - f1 * (x1 - x0) / (f1 - f0);
            if (std::fabs(x2 - x1) < tol) return {x2, k, true};
            x0 = x1; f0 = f1;
            x1 = x2; f1 = f(x1);
        }
        return {x1, max_iter, false};
    }

    Result regula_falsi(const Fn& f, double a, double b, bool illinois,
                        double tol = 1e-13, int max_iter = 200) {
        double fa = f(a), fb = f(b);
        int side = 0;
        double c = a;
        for (int k = 1; k <= max_iter; ++k) {
            double next_c = (fa * b - fb * a) / (fa - fb);
            if (std::fabs(next_c - c) < tol || std::fabs(b - a) < tol) return {next_c, k, true};
            c = next_c;
            double fc = f(c);
            if (std::fabs(fc) < tol) return {c, k, true};
            if ((fc < 0) == (fb < 0)) {
                b = c; fb = fc;
                if (illinois && side == -1) fa *= 0.5;
                side = -1;
            } else {
                a = c; fa = fc;
                if (illinois && side == +1) fb *= 0.5;
                side = +1;
            }
        }
        return {c, max_iter, false};
    }

} // namespace root_finding

int main() {
    using namespace root_finding;
    std::cout << "============================================================\n";
    std::cout << "        Secant and Illinois Regula Falsi Root Finding\n";
    std::cout << "============================================================\n\n";

    struct Problem {
        std::string name;
        Fn f;
        double a, b, exact;
    };
    Problem probs[] = {
        {"x^3 - x - 2 = 0",        [](double x) { return x * x * x - x - 2.0; }, 1.0, 2.0, 1.5213797068045676},
        {"cos(x) - x = 0",         [](double x) { return std::cos(x) - x; }, 0.0, 1.0, 0.7390851332151607},
        {"x^2 - 2 = 0 (sqrt 2)",   [](double x) { return x * x - 2.0; }, 1.0, 2.0, std::sqrt(2.0)},
        {"e^x - 3x = 0",           [](double x) { return std::exp(x) - 3.0 * x; }, 0.0, 1.0, 0.6190612867359451},
        {"x^10 - 1 = 0 (curved)",  [](double x) { return std::pow(x, 10) - 1.0; }, 0.8, 1.2, 1.0},
    };

    std::cout << std::left << std::setw(24) << "Equation"
              << std::setw(20) << "Root (Secant)"
              << std::setw(9) << "Secant"
              << std::setw(9) << "RFalsi"
              << "Illinois\n" << std::string(72, '-') << "\n";

    bool ok = true;
    for (auto& p : probs) {
        auto rs = secant(p.f, p.a, p.b);
        auto rf = regula_falsi(p.f, p.a, p.b, false);
        auto ri = regula_falsi(p.f, p.a, p.b, true);
        if (std::fabs(rs.root - p.exact) > 1e-10 || std::fabs(ri.root - p.exact) > 1e-10) ok = false;
        std::cout << std::setw(24) << p.name
                  << std::setw(20) << std::fixed << std::setprecision(14) << rs.root
                  << std::setw(9) << rs.iters
                  << std::setw(9) << rf.iters
                  << ri.iters << "\n";
    }
    std::cout << "\nNotice on x^10 - 1: plain Regula Falsi stagnates while Illinois converges fast.\n";
    std::cout << "Verification against exact roots: " << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
