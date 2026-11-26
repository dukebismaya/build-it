/*
 * ============================================================
 *       Horner's Method, Ruffini's Rule & Taylor Shift
 * ============================================================
 *
 * Description:
 *   Given a degree-n polynomial P(x) = a_n x^n + ... + a_1 x + a_0:
 *     1. Horner's Method evaluates P(x_0) in n multiplications and
 *        n additions:
 *          b_n = a_n,  b_k = b_{k+1} * x_0 + a_k   (k = n-1..0)
 *        giving P(x_0) = b_0.
 *
 *     2. Ruffini's Rule observes that the intermediate values
 *        Q(x) = b_n x^{n-1} + ... + b_1 form the exact quotient of
 *        synthetic division:
 *          P(x) = (x - x_0) Q(x) + b_0
 *
 *     3. Iterated Horner (Complete Taylor Shift):
 *        Repeating synthetic division on the successive quotients
 *        yields all normalized derivatives c_k = P^{(k)}(x_0) / k!
 *        so that P(x_0 + h) = sum_{k=0}^n c_k * h^k, which also
 *        evaluates P'(x_0) and P''(x_0) for Halley / Newton root
 *        polishers.
 *
 * Complexity:
 *   Single evaluation + quotient: O(N)
 *   Full Taylor shift (all derivatives at x_0): O(N^2) in-place.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <iomanip>

namespace horner_ruffini {

    // coeffs[k] is the coefficient of x^k (index 0 = constant term, index n = leading term)
    struct DivisionResult {
        double remainder;            // P(x0)
        std::vector<double> quotient; // Q(x) where P(x) = (x - x0)*Q(x) + remainder
    };

    DivisionResult synthetic_division(const std::vector<double>& a, double x0) {
        int n = static_cast<int>(a.size()) - 1;
        if (n == 0) return {a[0], {}};
        std::vector<double> q(n);
        double cur = a[n];
        for (int k = n - 1; k >= 0; --k) {
            q[k] = cur;
            cur = cur * x0 + a[k];
        }
        return {cur, q};
    }

    // In-place iterated Horner: returns c[k] = P^(k)(x0) / k! so P(x0 + h) = sum c[k] h^k
    std::vector<double> taylor_shift(std::vector<double> c, double x0) {
        int n = static_cast<int>(c.size()) - 1;
        for (int pass = 0; pass < n; ++pass)
            for (int k = n - 1; k >= pass; --k)
                c[k] += c[k + 1] * x0;
        return c;
    }

    double eval_horner(const std::vector<double>& a, double x) {
        double r = 0.0;
        for (int k = static_cast<int>(a.size()) - 1; k >= 0; --k)
            r = r * x + a[k];
        return r;
    }

} // namespace horner_ruffini

int main() {
    using namespace horner_ruffini;
    std::cout << "============================================================\n";
    std::cout << "       Horner's Method, Ruffini's Rule & Taylor Shift\n";
    std::cout << "============================================================\n\n";

    // P(x) = 2x^4 - 3x^3 + 5x^2 - 7x + 11 (stored low-to-high degree)
    std::vector<double> P = {11.0, -7.0, 5.0, -3.0, 2.0};
    double x0 = 2.0;

    auto div = synthetic_division(P, x0);
    std::cout << "P(x) = 2x^4 - 3x^3 + 5x^2 - 7x + 11 at x0 = 2:\n";
    std::cout << "  P(2) = " << div.remainder << "\n  Quotient Q(x) coefficients (low to high): ";
    for (double q : div.quotient) std::cout << q << " ";

    auto shifted = taylor_shift(P, x0);
    std::cout << "\n  Taylor coefficients c_k = P^(k)(2)/k!: ";
    for (double c : shifted) std::cout << c << " ";
    std::cout << "\n  (P(2)=" << shifted[0] << ", P'(2)=" << shifted[1]
              << ", P''(2)=" << 2 * shifted[2] << ")\n";

    std::mt19937 rng(42);
    bool ok = true;
    for (int trial = 0; trial < 200 && ok; ++trial) {
        int deg = 1 + rng() % 7;
        std::vector<double> poly(deg + 1);
        for (auto& c : poly) c = (static_cast<int>(rng() % 21) - 10) * 0.5;
        double a = (static_cast<int>(rng() % 9) - 4) * 0.25;
        auto sd = synthetic_division(poly, a);
        auto ts = taylor_shift(poly, a);

        for (double x : {-1.25, -0.3, 0.5, 1.25}) {
            double px = eval_horner(poly, x);
            double from_div = (x - a) * eval_horner(sd.quotient, x) + sd.remainder;
            double from_taylor = eval_horner(ts, x - a);
            double tol = 1e-8 * std::max(1.0, std::fabs(px));
            if (std::fabs(px - from_div) > tol || std::fabs(px - from_taylor) > tol) ok = false;
        }
    }
    std::cout << "\nRandom polynomial division & Taylor shift check (200 trials): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
