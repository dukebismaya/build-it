/*
 * ============================================================
 *            Gauss-Legendre Algorithm for Pi
 * ============================================================
 *
 * Description:
 *   The Gauss-Legendre (Brent-Salamin) algorithm computes pi
 *   using the arithmetic-geometric mean. It converges
 *   QUADRATICALLY: the number of correct digits roughly doubles
 *   with every iteration.
 *
 * Iteration:
 *   a0 = 1, b0 = 1/sqrt(2), t0 = 1/4, p0 = 1
 *   a_{n+1} = (a_n + b_n) / 2
 *   b_{n+1} = sqrt(a_n * b_n)
 *   t_{n+1} = t_n - p_n * (a_n - a_{n+1})^2
 *   p_{n+1} = 2 * p_n
 *   pi ~ (a_{n+1} + b_{n+1})^2 / (4 * t_{n+1})
 *
 * Complexity:
 *   O(log D) iterations for D digits; each iteration costs one
 *   multiplication and one square root at the working precision.
 *   (Here we use long double, so ~3 iterations saturate precision.)
 * ============================================================
 */

#include <iostream>
#include <cmath>
#include <iomanip>

namespace gauss_legendre {

    struct State { long double a, b, t, p; };

    long double estimate(const State& s) {
        return (s.a + s.b) * (s.a + s.b) / (4.0L * s.t);
    }

    State step(const State& s) {
        State n;
        n.a = (s.a + s.b) / 2.0L;
        n.b = std::sqrt(s.a * s.b);
        n.t = s.t - s.p * (s.a - n.a) * (s.a - n.a);
        n.p = 2.0L * s.p;
        return n;
    }

} // namespace gauss_legendre

int main() {
    using namespace gauss_legendre;
    const long double PI_REF = 3.14159265358979323846264338327950288L;

    std::cout << "============================================================\n";
    std::cout << "            Gauss-Legendre Algorithm for Pi\n";
    std::cout << "============================================================\n\n";

    State s{1.0L, 1.0L / std::sqrt(2.0L), 0.25L, 1.0L};
    std::cout << std::setw(5) << "Iter" << std::setw(26) << "Estimate"
              << std::setw(16) << "Abs Error" << std::setw(14) << "Digits" << "\n";
    std::cout << std::string(61, '-') << "\n";

    for (int i = 1; i <= 5; ++i) {
        s = step(s);
        long double pi = estimate(s);
        long double err = std::fabs(pi - PI_REF);
        double digits = err > 0 ? -std::log10(static_cast<double>(err)) : 19.0;
        std::cout << std::setw(5) << i
                  << std::setw(26) << std::setprecision(20) << std::fixed << pi
                  << std::setw(16) << std::scientific << std::setprecision(2) << err
                  << std::setw(14) << std::fixed << std::setprecision(1) << digits << "\n";
    }

    std::cout << "\nCorrect digits roughly double each step: 1 -> 3 -> 8 -> 19 ...\n";
    std::cout << "Precision of long double is reached after ~3 iterations.\n";
    return 0;
}
