/*
 * ============================================================
 *            Newton-Raphson Root Finding Method
 * ============================================================
 *
 * Description:
 *   Newton's method finds a root of f(x) = 0 by repeatedly
 *   following the tangent line:
 *
 *     x_{k+1} = x_k - f(x_k) / f'(x_k)
 *
 *   Near a simple root it converges QUADRATICALLY:
 *     |e_{k+1}| ~ C * |e_k|^2
 *   At a root of multiplicity m it degrades to linear
 *   convergence; the modified step x - m f/f' restores it.
 *
 *   This program compares Newton against bisection and shows
 *   the effect of multiplicity.
 *
 * Complexity:
 *   O(log log(1/eps)) iterations near a simple root (Newton),
 *   O(log(1/eps)) for bisection.
 * ============================================================
 */

#include <iostream>
#include <cmath>
#include <functional>
#include <iomanip>
#include <string>

namespace newton {

    using Fn = std::function<double(double)>;

    struct Result { double root; int iters; bool converged; };

    Result solve(const Fn& f, const Fn& df, double x0, double m = 1.0,
                 double tol = 1e-14, int max_iter = 100, bool trace = false) {
        double x = x0;
        for (int k = 1; k <= max_iter; ++k) {
            double d = df(x);
            if (d == 0.0) return {x, k, false};
            double nx = x - m * f(x) / d;
            if (trace)
                std::cout << "    iter " << std::setw(2) << k << ": x = "
                          << std::setprecision(16) << std::fixed << nx << "\n";
            if (std::fabs(nx - x) < tol) return {nx, k, true};
            x = nx;
        }
        return {x, max_iter, false};
    }

    Result bisection(const Fn& f, double lo, double hi, double tol = 1e-14) {
        int it = 0;
        while (hi - lo > tol && it < 200) {
            double mid = 0.5 * (lo + hi);
            if ((f(lo) < 0) == (f(mid) < 0)) lo = mid; else hi = mid;
            ++it;
        }
        return {0.5 * (lo + hi), it, true};
    }

} // namespace newton

int main() {
    using namespace newton;
    std::cout << "============================================================\n";
    std::cout << "             Newton-Raphson Root Finding Method\n";
    std::cout << "============================================================\n\n";

    std::cout << "1) sqrt(2) as root of x^2 - 2 (trace, x0 = 1):\n";
    auto r1 = solve([](double x) { return x * x - 2; }, [](double x) { return 2 * x; }, 1.0, 1.0, 1e-15, 50, true);
    std::cout << "   error = " << std::scientific << std::fabs(r1.root - std::sqrt(2.0)) << "\n\n";

    std::cout << "2) Newton vs bisection on cos(x) - x:\n";
    Fn g = [](double x) { return std::cos(x) - x; };
    Fn dg = [](double x) { return -std::sin(x) - 1; };
    auto rn = solve(g, dg, 1.0);
    auto rb = bisection(g, 0.0, 1.0);
    std::cout << std::fixed << std::setprecision(15)
              << "   Newton   : " << rn.root << " in " << rn.iters << " iterations\n"
              << "   Bisection: " << rb.root << " in " << rb.iters << " iterations\n\n";

    std::cout << "3) Double root of (x - 1)^2 (x + 2):\n";
    Fn h = [](double x) { return (x - 1) * (x - 1) * (x + 2); };
    Fn dh = [](double x) { return 2 * (x - 1) * (x + 2) + (x - 1) * (x - 1); };
    auto plain = solve(h, dh, 3.0, 1.0, 1e-12, 200);
    auto modified = solve(h, dh, 3.0, 2.0, 1e-12, 200);
    std::cout << "   Standard (m=1): " << plain.root << " in " << plain.iters << " iterations (linear)\n";
    std::cout << "   Modified (m=2): " << modified.root << " in " << modified.iters << " iterations (quadratic)\n\n";

    std::cout << "4) Cube roots via f(x) = x^3 - a:\n";
    bool ok = true;
    for (double a : {2.0, 27.0, 1000.0, 0.125, 12345.678}) {
        auto r = solve([a](double x) { return x * x * x - a; }, [](double x) { return 3 * x * x; }, a > 1 ? a : 1.0);
        bool good = std::fabs(r.root - std::cbrt(a)) < 1e-10;
        ok = ok && good;
        std::cout << "   cbrt(" << std::setprecision(3) << a << ") = " << std::setprecision(12) << r.root
                  << " (" << r.iters << " iters) " << (good ? "OK" : "FAIL") << "\n";
    }
    std::cout << "\nVerification: " << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
