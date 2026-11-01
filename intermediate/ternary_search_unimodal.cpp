/**
 * @file ternary_search_unimodal.cpp
 * @brief Implements Ternary Search for finding extrema of unimodal functions on continuous and discrete domains.
 *
 * Theory:
 * A function f(x) is unimodal on [L, R] if it either strictly increases and then strictly decreases
 * (has a single maximum), or strictly decreases and then strictly increases (has a single minimum).
 *
 * Continuous Domain Algorithm:
 * - Divide [L, R] into three equal segments:
 *     m1 = L + (R - L) / 3.0
 *     m2 = R - (R - L) / 3.0
 * - To find MAXIMUM:
 *     If f(m1) < f(m2), the maximum cannot be in [L, m1] -> set L = m1
 *     Else, the maximum cannot be in [m2, R] -> set R = m2
 * - Repeat until |R - L| < EPS (e.g., 100 iterations gives ~10^-15 precision).
 *
 * Discrete Domain (Bitonic Array Peak Finding):
 * - Integer-based ternary search to find the peak index in O(log N) time.
 *
 * Time Complexity:
 * - Continuous: O(log_{1.5}((R - L) / EPS))
 * - Discrete:   O(log_{1.5}(N))
 * Space Complexity: O(1)
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <iomanip>

/**
 * @brief Continuous ternary search to find the x that maximizes f(x) on [L, R].
 */
double ternarySearchMaximize(double L, double R, const std::function<double(double)>& f, int iterations = 100) {
    for (int iter = 0; iter < iterations; ++iter) {
        double m1 = L + (R - L) / 3.0;
        double m2 = R - (R - L) / 3.0;

        if (f(m1) < f(m2)) {
            L = m1;
        } else {
            R = m2;
        }
    }
    return (L + R) / 2.0;
}

/**
 * @brief Continuous ternary search to find the x that minimizes f(x) on [L, R].
 */
double ternarySearchMinimize(double L, double R, const std::function<double(double)>& f, int iterations = 100) {
    for (int iter = 0; iter < iterations; ++iter) {
        double m1 = L + (R - L) / 3.0;
        double m2 = R - (R - L) / 3.0;

        if (f(m1) > f(m2)) {
            L = m1;
        } else {
            R = m2;
        }
    }
    return (L + R) / 2.0;
}

/**
 * @brief Discrete ternary search to find the peak index of a unimodal integer array.
 */
int discreteTernarySearchPeak(const std::vector<int>& arr) {
    int L = 0;
    int R = static_cast<int>(arr.size()) - 1;

    while (R - L >= 3) {
        int m1 = L + (R - L) / 3;
        int m2 = R - (R - L) / 3;

        if (arr[m1] < arr[m2]) {
            L = m1;
        } else {
            R = m2;
        }
    }

    // Linearly inspect the remaining small interval [L, R]
    int bestIdx = L;
    for (int i = L + 1; i <= R; ++i) {
        if (arr[i] > arr[bestIdx]) {
            bestIdx = i;
        }
    }
    return bestIdx;
}

int main() {
    std::cout << "=== Ternary Search for Unimodal Functions ===\n\n";

    // 1. Continuous Maximum Example: Parabola f(x) = -(x - 3.5)^2 + 12
    // Vertex is at x = 3.5 with maximum value = 12.0
    auto f1 = [](double x) {
        return -((x - 3.5) * (x - 3.5)) + 12.0;
    };

    double xMax = ternarySearchMaximize(-10.0, 10.0, f1);
    std::cout << "1. Continuous Maximization: f(x) = -(x - 3.5)^2 + 12 on [-10, 10]\n";
    std::cout << "   Computed x*: " << std::fixed << std::setprecision(8) << xMax
              << " (Expected: 3.50000000)\n";
    std::cout << "   Maximum f(x*): " << f1(xMax)
              << " (Expected: 12.00000000)\n\n";

    // 2. Continuous Minimum Example: f(x) = 2*x^2 - 12*x + 23
    // Vertex is at x = -(-12)/(2*2) = 3.0 with min value = 2*(9) - 36 + 23 = 5.0
    auto f2 = [](double x) {
        return 2.0 * x * x - 12.0 * x + 23.0;
    };

    double xMin = ternarySearchMinimize(-5.0, 8.0, f2);
    std::cout << "2. Continuous Minimization: f(x) = 2x^2 - 12x + 23 on [-5, 8]\n";
    std::cout << "   Computed x*: " << xMin << " (Expected: 3.00000000)\n";
    std::cout << "   Minimum f(x*): " << f2(xMin) << " (Expected: 5.00000000)\n\n";

    // 3. Discrete Unimodal Array Peak
    std::vector<int> mountain = {1, 4, 8, 15, 23, 42, 60, 55, 30, 12, 5, 2};
    std::cout << "3. Discrete Array Peak Finding:\n   Array: [ ";
    for (int v : mountain) std::cout << v << " ";
    std::cout << "]\n";

    int peakIdx = discreteTernarySearchPeak(mountain);
    std::cout << "   Peak Value: " << mountain[peakIdx] << " at index " << peakIdx << '\n';

    return 0;
}

