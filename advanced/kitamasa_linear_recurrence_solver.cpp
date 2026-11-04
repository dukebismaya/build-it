/**
 * @file kitamasa_linear_recurrence_solver.cpp
 * @brief Computes the N-th term of an order-K linear recurrence in O(K^2 log N) using the Kitamasa method.
 *
 * Theory (Fiduccia's Algorithm / Kitamasa Method):
 * Given a linear recurrence relation of order K:
 *   a_n = sum_{i=1}^K c_i * a_{n - i} (mod MOD)
 * with initial terms a_0, a_1, ..., a_{K-1}.
 *
 * Matrix exponentiation requires O(K^3 log N) operations.
 * The Kitamasa method reduces the complexity to O(K^2 log N) by working directly
 * with polynomial residue arithmetic modulo the characteristic polynomial:
 *   P(x) = x^K - c_1 * x^{K-1} - c_2 * x^{K-2} - ... - c_K
 *
 * Cayley-Hamilton Principle:
 * Any power x^N can be expressed as:
 *   x^N = Q(x) * P(x) + R(x)
 * where deg(R) < K.
 * Writing R(x) = sum_{j=0}^{K-1} r_j * x^j, the N-th term is simply:
 *   a_N = sum_{j=0}^{K-1} r_j * a_j (mod MOD)
 *
 * We compute x^N mod P(x) in O(K^2 log N) using repeated polynomial squaring.
 *
 * Time Complexity: O(K^2 * log(N))
 * Space Complexity: O(K)
 */

#include <iostream>
#include <vector>
#include <iomanip>

class KitamasaSolver {
private:
    long long MOD;

    // Multiplies two polynomials of degree < K and reduces modulo P(x)
    std::vector<long long> multiplyMod(
        const std::vector<long long>& A,
        const std::vector<long long>& B,
        const std::vector<long long>& C) const {
        int K = static_cast<int>(C.size());
        std::vector<long long> product(2 * K, 0);

        // Standard polynomial multiplication: deg < 2K
        for (int i = 0; i < K; ++i) {
            if (A[i] == 0) continue;
            for (int j = 0; j < K; ++j) {
                product[i + j] = (product[i + j] + A[i] * B[j]) % MOD;
            }
        }

        // Reduce from highest power down to K using x^K = sum_{i=1}^K c_i * x^{K-i}
        for (int i = 2 * K - 2; i >= K; --i) {
            long long factor = product[i];
            if (factor == 0) continue;

            for (int j = 1; j <= K; ++j) {
                int targetDeg = i - j;
                product[targetDeg] = (product[targetDeg] + factor * C[j - 1]) % MOD;
            }
            product[i] = 0;
        }

        product.resize(K);
        return product;
    }

public:
    explicit KitamasaSolver(long long modulo = 1000000007LL) : MOD(modulo) {}

    /**
     * @brief Computes a_N modulo MOD.
     * @param initialTerms [a_0, a_1, ..., a_{K-1}].
     * @param recurrenceCoeffs [c_1, c_2, ..., c_K] where a_n = c_1*a_{n-1} + ... + c_K*a_{n-K}.
     * @param N The query index.
     */
    long long solve(
        const std::vector<long long>& initialTerms,
        const std::vector<long long>& recurrenceCoeffs,
        long long N) const {
        int K = static_cast<int>(initialTerms.size());
        if (N < K) {
            return (initialTerms[N] % MOD + MOD) % MOD;
        }

        // Compute x^N mod P(x) via binary exponentiation
        // res = 1 = x^0
        std::vector<long long> res(K, 0);
        res[0] = 1;

        // base = x^1
        std::vector<long long> base(K, 0);
        if (K > 1) {
            base[1] = 1;
        } else {
            base[0] = recurrenceCoeffs[0] % MOD;
        }

        long long exp = N;
        while (exp > 0) {
            if (exp & 1) {
                res = multiplyMod(res, base, recurrenceCoeffs);
            }
            base = multiplyMod(base, base, recurrenceCoeffs);
            exp >>= 1;
        }

        // a_N = sum_{j=0}^{K-1} res[j] * a_j
        long long ans = 0;
        for (int j = 0; j < K; ++j) {
            ans = (ans + res[j] * (initialTerms[j] % MOD)) % MOD;
        }

        return (ans % MOD + MOD) % MOD;
    }
};

int main() {
    std::cout << "=== Kitamasa Method for Linear Recurrences (O(K^2 log N)) ===\n\n";

    long long MOD = 1000000007LL;
    KitamasaSolver solver(MOD);

    // 1. Fibonacci Recurrence: F_n = 1*F_{n-1} + 1*F_{n-2}
    // F_0 = 0, F_1 = 1
    std::vector<long long> fibInit = {0, 1};
    std::vector<long long> fibCoeffs = {1, 1};

    std::cout << "1. Fibonacci Recurrence:\n";
    std::vector<long long> fibQueries = {10, 50, 100, 1000, 1000000000000000000LL}; // 10^18

    for (long long n : fibQueries) {
        long long val = solver.solve(fibInit, fibCoeffs, n);
        std::cout << "   F(" << std::setw(18) << n << ") % (10^9+7) = " << val << '\n';
    }

    // 2. Order-4 Recurrence:
    // a_n = 2*a_{n-1} + 0*a_{n-2} - 1*a_{n-3} + 3*a_{n-4}
    // Initial: a_0 = 1, a_1 = 2, a_2 = 3, a_3 = 4
    std::vector<long long> order4Init = {1, 2, 3, 4};
    std::vector<long long> order4Coeffs = {2, 0, -1, 3};

    std::cout << "\n2. Order-4 Linear Recurrence:\n";
    std::cout << "   a_n = 2*a_{n-1} - a_{n-3} + 3*a_{n-4} (mod 10^9+7)\n";

    for (int i = 4; i <= 8; ++i) {
        long long val = solver.solve(order4Init, order4Coeffs, i);
        std::cout << "   a(" << i << ") = " << val << '\n';
    }

    long long hugeResult = solver.solve(order4Init, order4Coeffs, 1000000000000LL);
    std::cout << "   a(10^12) % (10^9+7) = " << hugeResult << '\n';

    return 0;
}
