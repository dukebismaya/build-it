/*
 * Program Description:
 * Tribonacci Sequence Generator and Analyzer in C++.
 * Similar to Fibonacci, but each number is the sum of the preceding three terms:
 * T_0 = 0, T_1 = 1, T_2 = 1
 * T_n = T_{n-1} + T_{n-2} + T_{n-3} for n >= 3.
 *
 * Sequence starts: 0, 1, 1, 2, 4, 7, 13, 24, 44, 81, 149, 274, 504...
 *
 * Concepts covered:
 * 1. Iterative sliding window in O(N) time and O(1) space.
 * 2. Ratio convergence towards the Tribonacci Constant (approx 1.839286755...).
 * 3. 3x3 Matrix Exponentiation for computing T_N in O(log N) time.
 */

#include <iostream>
#include <vector>
#include <iomanip>

// Iterative computation in O(N) time and O(1) space
long long tribonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1 || n == 2) return 1;

    long long t0 = 0, t1 = 1, t2 = 1;
    for (int i = 3; i <= n; ++i) {
        long long t3 = t0 + t1 + t2;
        t0 = t1;
        t1 = t2;
        t2 = t3;
    }
    return t2;
}

// Generates the first N terms of the Tribonacci sequence
std::vector<long long> generateTribonacciSequence(int count) {
    std::vector<long long> seq;
    if (count <= 0) return seq;

    seq.reserve(count);
    for (int i = 0; i < count; ++i) {
        if (i == 0) seq.push_back(0);
        else if (i == 1 || i == 2) seq.push_back(1);
        else seq.push_back(seq[i - 1] + seq[i - 2] + seq[i - 3]);
    }
    return seq;
}

// 3x3 Matrix multiplication
std::vector<std::vector<long long>> multiplyMatrix3x3(
    const std::vector<std::vector<long long>>& A,
    const std::vector<std::vector<long long>>& B) {
    std::vector<std::vector<long long>> C(3, std::vector<long long>(3, 0));
    for (int i = 0; i < 3; ++i) {
        for (int k = 0; k < 3; ++k) {
            for (int j = 0; j < 3; ++j) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

// 3x3 Matrix Exponentiation in O(log N)
long long tribonacciMatrixLogN(int n) {
    if (n <= 0) return 0;
    if (n == 1 || n == 2) return 1;

    // Transition matrix:
    // [ T_{n+1} ]   [ 1 1 1 ] [ T_n     ]
    // [ T_n     ] = [ 1 0 0 ] [ T_{n-1} ]
    // [ T_{n-1} ]   [ 0 1 0 ] [ T_{n-2} ]
    std::vector<std::vector<long long>> T = {
        {1, 1, 1},
        {1, 0, 0},
        {0, 1, 0}
    };

    std::vector<std::vector<long long>> res = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };

    int exp = n - 2;
    while (exp > 0) {
        if (exp & 1) res = multiplyMatrix3x3(res, T);
        T = multiplyMatrix3x3(T, T);
        exp >>= 1;
    }

    // [T_2, T_1, T_0]^T = [1, 1, 0]^T
    return res[0][0] * 1 + res[0][1] * 1 + res[0][2] * 0;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "       Tribonacci Sequence Generator      \n";
    std::cout << "==========================================\n\n";

    int count = 15;
    std::cout << "First " << count << " Tribonacci Numbers:\n";
    auto seq = generateTribonacciSequence(count);

    for (int i = 0; i < count; ++i) {
        std::cout << "T(" << std::setw(2) << i << ") = " << std::setw(5) << seq[i];
        if (i >= 3 && seq[i - 1] > 0) {
            double ratio = static_cast<double>(seq[i]) / seq[i - 1];
            std::cout << std::fixed << std::setprecision(6) << "  (Ratio: " << ratio << ")";
        }
        std::cout << "\n";
    }

    std::cout << "\nTribonacci Constant limit: ~1.839287\n\n";

    std::cout << "--- O(log N) Matrix Exponentiation Verification ---\n";
    std::vector<int> testIndices = {10, 15, 20, 25, 30};
    for (int idx : testIndices) {
        long long linear = tribonacci(idx);
        long long fast = tribonacciMatrixLogN(idx);
        std::cout << "T(" << idx << "): Linear = " << linear << ", Matrix O(log N) = " << fast
                  << " -> " << (linear == fast ? "[MATCH]" : "[MISMATCH]") << "\n";
    }

    return 0;
}
