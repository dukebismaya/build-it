/*
 * ============================================================
 *           Hofstadter Q-Sequence Generator
 * ============================================================
 *
 * Description:
 *   Introduced in "Godel, Escher, Bach", the Q-sequence is a
 *   "meta-Fibonacci" recurrence where earlier terms decide
 *   WHICH earlier terms are added:
 *
 *     Q(1) = Q(2) = 1
 *     Q(n) = Q(n - Q(n-1)) + Q(n - Q(n-2))   for n > 2
 *
 *   1, 1, 2, 3, 3, 4, 5, 5, 6, 6, 6, 8, 8, 8, 10, 9, 10, ...
 *
 *   The sequence behaves chaotically; it is not even known
 *   whether it is defined for all n. This program also shows
 *   the related Hofstadter Female/Male (F/M) sequences:
 *     F(0) = 1, M(0) = 0
 *     F(n) = n - M(F(n-1)),  M(n) = n - F(M(n-1))
 *
 * Complexity: O(N) time and space (bottom-up).
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <iomanip>

namespace hofstadter {

    std::vector<int> q_sequence(int n) {
        std::vector<int> q(n + 1, 0);
        q[1] = 1; if (n >= 2) q[2] = 1;
        for (int i = 3; i <= n; ++i)
            q[i] = q[i - q[i - 1]] + q[i - q[i - 2]];
        return q;
    }

    void female_male(int n, std::vector<int>& F, std::vector<int>& M) {
        F.assign(n + 1, 0); M.assign(n + 1, 0);
        F[0] = 1; M[0] = 0;
        for (int i = 1; i <= n; ++i) {
            F[i] = i - M[F[i - 1]];
            M[i] = i - F[M[i - 1]];
        }
    }

} // namespace hofstadter

int main() {
    std::cout << "============================================================\n";
    std::cout << "             Hofstadter Q-Sequence Generator\n";
    std::cout << "============================================================\n\n";

    auto q = hofstadter::q_sequence(100000);
    std::cout << "Q(1..40):\n";
    for (int i = 1; i <= 40; ++i) {
        std::cout << std::setw(4) << q[i];
        if (i % 20 == 0) std::cout << "\n";
    }

    const int expected[] = {1, 1, 2, 3, 3, 4, 5, 5, 6, 6, 6, 8, 8, 8, 10, 9, 10, 11, 11, 12};
    bool ok = true;
    for (int i = 0; i < 20; ++i) if (q[i + 1] != expected[i]) ok = false;
    std::cout << "\nFirst 20 match OEIS A005185: " << (ok ? "PASSED" : "FAILED") << "\n";

    std::cout << "\nQ(n)/n hovers around 1/2 (chaotically):\n";
    for (int n : {10, 100, 1000, 10000, 100000})
        std::cout << "  n = " << std::setw(6) << n << ": Q(n) = " << std::setw(6) << q[n]
                  << ", Q(n)/n = " << std::fixed << std::setprecision(4) << double(q[n]) / n << "\n";

    std::vector<int> F, M;
    hofstadter::female_male(20, F, M);
    std::cout << "\nFemale F(0..20): ";
    for (int x : F) std::cout << x << " ";
    std::cout << "\nMale   M(0..20): ";
    for (int x : M) std::cout << x << " ";
    std::cout << "\n";
    return 0;
}
