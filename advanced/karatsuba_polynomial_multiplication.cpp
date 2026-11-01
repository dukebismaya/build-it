/**
 * @file karatsuba_polynomial_multiplication.cpp
 * @brief Multiplies polynomials using the Karatsuba divide-and-conquer algorithm in O(N^1.585) time.
 *
 * Background:
 * Discovered by Anatoly Karatsuba in 1960, this was the first algorithm to break the
 * O(N^2) barrier for multiplication.
 *
 * Algorithm:
 * Let A(x) and B(x) be polynomials of degree < N.
 * Split each into high and low halves of size k = N / 2:
 *   A(x) = A1 * x^k + A0
 *   B(x) = B1 * x^k + B0
 *
 * Compute 3 recursive multiplications instead of 4:
 *   1. P0 = A0 * B0
 *   2. P2 = A1 * B1
 *   3. P1 = (A0 + A1) * (B0 + B1) - P0 - P2
 *
 * Then the product is:
 *   C(x) = P2 * x^(2k) + P1 * x^k + P0
 *
 * Recurrence: T(N) = 3 * T(N/2) + O(N) => O(N^{log2(3)}) ~ O(N^1.585)
 *
 * Time Complexity: O(N^1.585)
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

using Poly = std::vector<long long>;

/**
 * @brief Standard naive O(N^2) polynomial multiplication.
 */
Poly naiveMultiply(const Poly& A, const Poly& B) {
    if (A.empty() || B.empty()) return {};
    Poly C(A.size() + B.size() - 1, 0);
    for (size_t i = 0; i < A.size(); ++i) {
        for (size_t j = 0; j < B.size(); ++j) {
            C[i + j] += A[i] * B[j];
        }
    }
    return C;
}

Poly addPoly(const Poly& A, const Poly& B) {
    Poly C(std::max(A.size(), B.size()), 0);
    for (size_t i = 0; i < A.size(); ++i) C[i] += A[i];
    for (size_t i = 0; i < B.size(); ++i) C[i] += B[i];
    return C;
}

Poly subtractPoly(const Poly& A, const Poly& B) {
    Poly C(std::max(A.size(), B.size()), 0);
    for (size_t i = 0; i < A.size(); ++i) C[i] += A[i];
    for (size_t i = 0; i < B.size(); ++i) C[i] -= B[i];
    return C;
}

/**
 * @brief Karatsuba polynomial multiplication.
 */
Poly karatsubaMultiply(const Poly& A, const Poly& B) {
    size_t n = std::max(A.size(), B.size());

    // Base case: for small polynomials, naive multiplication has lower overhead
    if (n <= 16) {
        return naiveMultiply(A, B);
    }

    size_t k = (n + 1) / 2;

    // Split A into A0 and A1
    Poly A0(A.begin(), A.begin() + std::min(A.size(), k));
    Poly A1;
    if (A.size() > k) {
        A1.assign(A.begin() + k, A.end());
    }

    // Split B into B0 and B1
    Poly B0(B.begin(), B.begin() + std::min(B.size(), k));
    Poly B1;
    if (B.size() > k) {
        B1.assign(B.begin() + k, B.end());
    }

    Poly P0 = karatsubaMultiply(A0, B0); // A0 * B0
    Poly P2 = karatsubaMultiply(A1, B1); // A1 * B1

    Poly sumA = addPoly(A0, A1);
    Poly sumB = addPoly(B0, B1);
    Poly P1 = karatsubaMultiply(sumA, sumB); // (A0 + A1) * (B0 + B1)

    P1 = subtractPoly(P1, P0);
    P1 = subtractPoly(P1, P2); // P1 = (A0+A1)(B0+B1) - P0 - P2

    // Combine: C(x) = P2 * x^(2k) + P1 * x^k + P0
    Poly result(A.size() + B.size() - 1, 0);

    for (size_t i = 0; i < P0.size(); ++i) {
        if (i < result.size()) result[i] += P0[i];
    }
    for (size_t i = 0; i < P1.size(); ++i) {
        if (i + k < result.size()) result[i + k] += P1[i];
    }
    for (size_t i = 0; i < P2.size(); ++i) {
        if (i + 2 * k < result.size()) result[i + 2 * k] += P2[i];
    }

    return result;
}

void printPolynomial(const Poly& p) {
    bool first = true;
    for (size_t i = 0; i < p.size(); ++i) {
        if (p[i] == 0) continue;
        if (!first && p[i] > 0) std::cout << " + ";
        else if (p[i] < 0) std::cout << " - ";
        first = false;

        long long coeff = std::abs(p[i]);
        if (i == 0) {
            std::cout << coeff;
        } else if (i == 1) {
            if (coeff != 1) std::cout << coeff;
            std::cout << "x";
        } else {
            if (coeff != 1) std::cout << coeff;
            std::cout << "x^" << i;
        }
    }
    if (first) std::cout << "0";
    std::cout << '\n';
}

int main() {
    std::cout << "=== Karatsuba Polynomial Multiplication (O(N^1.585)) ===\n\n";

    // 1. Small test case:
    // A(x) = 1 + 2x + 3x^2
    // B(x) = 4 + 5x + 6x^2
    Poly A = {1, 2, 3};
    Poly B = {4, 5, 6};

    std::cout << "Polynomial A(x): ";
    printPolynomial(A);
    std::cout << "Polynomial B(x): ";
    printPolynomial(B);

    Poly naiveRes = naiveMultiply(A, B);
    Poly karatsubaRes = karatsubaMultiply(A, B);

    std::cout << "\nProduct C(x) (Naive O(N^2)):     ";
    printPolynomial(naiveRes);

    std::cout << "Product C(x) (Karatsuba O(N^1.585)): ";
    printPolynomial(karatsubaRes);

    std::cout << "Verification: " << (naiveRes == karatsubaRes ? "PASS (Exact Match)" : "FAIL") << "\n\n";

    // 2. Larger polynomial test
    Poly A2 = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    Poly B2 = {2, 7, 1, 8, 2, 8, 1, 8, 2, 8, 4};

    Poly resA2B2 = karatsubaMultiply(A2, B2);
    Poly naiveA2B2 = naiveMultiply(A2, B2);

    std::cout << "Degree " << (A2.size() - 1) << " x Degree " << (B2.size() - 1) << " Test:\n";
    std::cout << "Karatsuba result coefficients: [ ";
    for (long long c : resA2B2) std::cout << c << " ";
    std::cout << "]\n";
    std::cout << "Match with naive method: " << (resA2B2 == naiveA2B2 ? "PASS" : "FAIL") << '\n';

    return 0;
}

