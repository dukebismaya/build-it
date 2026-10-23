/*
 * Program Description:
 * Lucas Sequence Generator and Mathematical Analyzer in C++.
 * The Lucas sequence is an integer sequence named after mathematician Édouard Lucas.
 * It follows the same recursive formula as Fibonacci, but with different initial values:
 * L_0 = 2, L_1 = 1
 * L_n = L_{n-1} + L_{n-2} for n >= 2.
 *
 * Sequence: 2, 1, 3, 4, 7, 11, 18, 29, 47, 76, 123, 199, 322...
 *
 * Concepts covered:
 * 1. Iterative sliding window in O(N) time and O(1) space.
 * 2. Identity linking Lucas and Fibonacci numbers:
 *    L_n = F_{n-1} + F_{n+1}
 * 3. Ratio convergence towards Golden Ratio phi = (1 + sqrt(5)) / 2 approx 1.618034.
 */

#include <iostream>
#include <vector>
#include <iomanip>

// Computes N-th Lucas number iteratively in O(N) time and O(1) space
long long lucasNumber(int n) {
    if (n <= 0) return 2;
    if (n == 1) return 1;

    long long prev2 = 2;
    long long prev1 = 1;
    long long current = 0;

    for (int i = 2; i <= n; ++i) {
        current = prev1 + prev2;
        prev2 = prev1;
        prev1 = current;
    }

    return current;
}

// Computes N-th Fibonacci number
long long fibonacciNumber(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;

    long long a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        long long c = a + b;
        a = b;
        b = c;
    }
    return b;
}

// Generates first N terms of the Lucas sequence
std::vector<long long> generateLucasSequence(int count) {
    std::vector<long long> seq;
    if (count <= 0) return seq;

    seq.reserve(count);
    for (int i = 0; i < count; ++i) {
        seq.push_back(lucasNumber(i));
    }
    return seq;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "         Lucas Sequence Generator         \n";
    std::cout << "==========================================\n\n";

    int count = 15;
    std::cout << "First " << count << " Lucas Numbers:\n";
    auto seq = generateLucasSequence(count);

    for (int i = 0; i < count; ++i) {
        std::cout << "L(" << std::setw(2) << i << ") = " << std::setw(5) << seq[i];
        if (i >= 2 && seq[i - 1] > 0) {
            double ratio = static_cast<double>(seq[i]) / seq[i - 1];
            std::cout << std::fixed << std::setprecision(6) << "  (Ratio: " << ratio << ")";
        }
        std::cout << "\n";
    }

    std::cout << "\nGolden Ratio Limit (phi): ~1.618034\n\n";

    std::cout << "--- Mathematical Verification: L_n = F_{n-1} + F_{n+1} ---\n";
    for (int n = 1; n <= 8; ++n) {
        long long l_n = lucasNumber(n);
        long long fibSum = fibonacciNumber(n - 1) + fibonacciNumber(n + 1);
        std::cout << "n = " << n << ": L(" << n << ") = " << l_n
                  << "  vs  F(" << n - 1 << ") + F(" << n + 1 << ") = "
                  << fibonacciNumber(n - 1) << " + " << fibonacciNumber(n + 1)
                  << " = " << fibSum
                  << " " << (l_n == fibSum ? "[VERIFIED]" : "[FAIL]") << "\n";
    }

    return 0;
}
