/**
 * @file matrix_exponentiation_fibonacci.cpp
 * @brief Computes N-th Fibonacci numbers and general linear recurrences in O(log N) via Matrix Exponentiation.
 *
 * Theory:
 * The Fibonacci recurrence F(n) = F(n-1) + F(n-2) with F(0) = 0, F(1) = 1
 * can be represented in matrix form:
 *   [ F(n+1)   F(n)   ]   [ 1  1 ]^n
 *   [ F(n)     F(n-1) ] = [ 1  0 ]
 *
 * Matrix Binary Exponentiation:
 * Computing M^n by repeated squaring takes O(K^3 * log(n)) operations,
 * where K is the matrix dimension (K = 2 for standard Fibonacci).
 * This allows computing F(10^18) % MOD in under a millisecond.
 *
 * Time Complexity: O(K^3 * log(N)) where K = 2
 * Space Complexity: O(K^2) for matrix multiplication
 */

#include <iostream>
#include <vector>
#include <iomanip>

class MatrixExpoFibonacci {
private:
    long long MOD;

    // Multiply two 2x2 matrices modulo MOD
    std::vector<std::vector<long long>> multiply(
        const std::vector<std::vector<long long>>& A,
        const std::vector<std::vector<long long>>& B) const {
        std::vector<std::vector<long long>> C(2, std::vector<long long>(2, 0));
        for (int i = 0; i < 2; ++i) {
            for (int k = 0; k < 2; ++k) {
                for (int j = 0; j < 2; ++j) {
                    C[i][j] = (C[i][j] + A[i][k] * B[k][j]) % MOD;
                }
            }
        }
        return C;
    }

    // Binary exponentiation of 2x2 matrix
    std::vector<std::vector<long long>> power(
        std::vector<std::vector<long long>> M, long long exp) const {
        std::vector<std::vector<long long>> res = {
            {1, 0},
            {0, 1}
        };

        while (exp > 0) {
            if (exp & 1) {
                res = multiply(res, M);
            }
            M = multiply(M, M);
            exp >>= 1;
        }

        return res;
    }

public:
    explicit MatrixExpoFibonacci(long long modulo = 1000000007LL) : MOD(modulo) {}

    /**
     * @brief Computes F(n) modulo MOD in O(log n).
     * @param n Index (n >= 0).
     * @return F(n) % MOD.
     */
    long long getFibonacci(long long n) const {
        if (n == 0) return 0;
        if (n == 1) return 1;

        std::vector<std::vector<long long>> T = {
            {1, 1},
            {1, 0}
        };

        auto Tn = power(T, n);
        return Tn[0][1]; // or Tn[1][0]
    }
};

int main() {
    std::cout << "=== N-th Fibonacci via Matrix Exponentiation (O(log N)) ===\n\n";

    long long MOD = 1000000007LL; // 10^9 + 7
    MatrixExpoFibonacci fibSolver(MOD);

    // 1. Verify small terms against standard iterative Fibonacci
    std::vector<long long> smallTerms(16);
    smallTerms[0] = 0;
    smallTerms[1] = 1;
    for (int i = 2; i < 16; ++i) {
        smallTerms[i] = (smallTerms[i - 1] + smallTerms[i - 2]) % MOD;
    }

    std::cout << "1. Verification of Small Terms (0 to 15):\n";
    std::cout << "  " << std::setw(6) << "n"
              << std::setw(16) << "Matrix Expo"
              << std::setw(16) << "Iterative Check"
              << std::setw(10) << "Match?" << '\n';
    std::cout << "  " << std::string(48, '-') << '\n';

    for (int i = 0; i < 16; ++i) {
        long long val = fibSolver.getFibonacci(i);
        std::cout << "  " << std::setw(6) << i
                  << std::setw(16) << val
                  << std::setw(16) << smallTerms[i]
                  << std::setw(10) << (val == smallTerms[i] ? "PASS" : "FAIL") << '\n';
    }

    // 2. Large powers of 10
    std::cout << "\n2. Huge Fibonacci Terms (modulo 10^9 + 7):\n";
    std::vector<long long> hugeN = {
        100LL,
        1000LL,
        1000000LL,
        1000000000LL,
        1000000000000000000LL // 10^18
    };

    std::cout << "  " << std::setw(24) << "n"
              << std::setw(24) << "F(n) % (10^9 + 7)" << '\n';
    std::cout << "  " << std::string(48, '-') << '\n';

    for (long long n : hugeN) {
        long long val = fibSolver.getFibonacci(n);
        std::cout << "  " << std::setw(24) << n
                  << std::setw(24) << val << '\n';
    }

    return 0;
}

