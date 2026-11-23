/*
 * ============================================================
 *              Pell and Pell-Lucas Number Generator
 * ============================================================
 *
 * Description:
 *   Pell numbers:       P0 = 0, P1 = 1, Pn = 2*P(n-1) + P(n-2)
 *     0, 1, 2, 5, 12, 29, 70, 169, 408, 985, ...
 *   Pell-Lucas numbers: Q0 = 2, Q1 = 2, Qn = 2*Q(n-1) + Q(n-2)
 *     2, 2, 6, 14, 34, 82, 198, 478, ...
 *
 *   The ratios (Q_n / 2) / P_n converge to sqrt(2), giving the
 *   best rational approximations 1/1, 3/2, 7/5, 17/12, 41/29, ...
 *   They also solve Pell's equation x^2 - 2y^2 = +-1 with
 *   x = Q_n / 2, y = P_n.
 *
 * Complexity: O(N) time, O(1) extra space per term.
 * ============================================================
 */

#include <iostream>
#include <cstdint>
#include <cmath>
#include <iomanip>

int main() {
    std::cout << "============================================================\n";
    std::cout << "            Pell and Pell-Lucas Number Generator\n";
    std::cout << "============================================================\n\n";

    const int N = 30;
    int64_t p0 = 0, p1 = 1, q0 = 2, q1 = 2;

    std::cout << std::setw(3) << "n" << std::setw(16) << "P(n)" << std::setw(16) << "Q(n)"
              << std::setw(22) << "(Q/2)/P ~ sqrt2" << std::setw(14) << "x^2-2y^2" << "\n";
    std::cout << std::string(71, '-') << "\n";

    bool ok = true;
    for (int n = 0; n <= N; ++n) {
        int64_t P = (n == 0) ? p0 : p1;
        int64_t Q = (n == 0) ? q0 : q1;
        int64_t x = Q / 2, y = P;
        int64_t pell = x * x - 2 * y * y;
        if (pell != 1 && pell != -1) ok = false;

        std::cout << std::setw(3) << n << std::setw(16) << P << std::setw(16) << Q;
        if (P) std::cout << std::setw(22) << std::fixed << std::setprecision(15) << double(x) / P;
        else   std::cout << std::setw(22) << "-";
        std::cout << std::setw(14) << pell << "\n";

        if (n > 0) {
            int64_t np = 2 * p1 + p0; p0 = p1; p1 = np;
            int64_t nq = 2 * q1 + q0; q0 = q1; q1 = nq;
        }
    }

    std::cout << "\nsqrt(2) = " << std::setprecision(15) << std::sqrt(2.0) << "\n";
    std::cout << "All terms satisfy x^2 - 2y^2 = +-1: " << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
