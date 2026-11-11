/**
 * @file kahan_compensated_summation.cpp
 * @brief Kahan and Neumaier Compensated Summation for floating-point accuracy in C++17.
 *
 * Floating-Point Error Accumulation:
 *   When summing N numbers in finite precision (e.g., IEEE 754 float with machine epsilon ~1.19e-7),
 *   naive summation accumulates rounding error proportional to O(N * epsilon) or even O(N) under
 *   large-plus-small magnitude differences.
 *
 * Kahan Summation Algorithm (1965):
 *   Maintains a separate running compensation variable `c` to capture lost low-order bits:
 *     y = input[i] - c
 *     t = sum + y
 *     c = (t - sum) - y
 *     sum = t
 *   Achieves O(1) error bound independent of N (unless terms oscillate wildly).
 *
 * Neumaier Improvement (1974):
 *   Handles cases where |input[i]| > |sum| correctly (which breaks standard Kahan):
 *     t = sum + input[i]
 *     if |sum| >= |input[i]|: c += (sum - t) + input[i]
 *     else:                    c += (input[i] - t) + sum
 *     sum = t
 *   Final result: sum + c.
 *
 * Complexity:
 *   - Time: O(N)
 *   - Space: O(1)
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

namespace numerical {

/**
 * @brief Naive floating-point summation.
 */
template <typename T>
T naiveSum(const std::vector<T>& arr) {
    T sum = T(0);
    for (T x : arr) {
        sum += x;
    }
    return sum;
}

/**
 * @brief Kahan compensated summation algorithm.
 */
template <typename T>
T kahanSum(const std::vector<T>& arr) {
    T sum = T(0);
    T c = T(0); // Running compensation for lost low-order bits

    for (T x : arr) {
        T y = x - c;
        T t = sum + y;
        c = (t - sum) - y;
        sum = t;
    }
    return sum;
}

/**
 * @brief Neumaier improved compensated summation algorithm.
 */
template <typename T>
T neumaierSum(const std::vector<T>& arr) {
    T sum = T(0);
    T c = T(0);

    for (T x : arr) {
        T t = sum + x;
        if (std::abs(sum) >= std::abs(x)) {
            c += (sum - t) + x;
        } else {
            c += (x - t) + sum;
        }
        sum = t;
    }
    return sum + c;
}

} // namespace numerical

int main() {
    std::cout << "========================================\n";
    std::cout << "   KAHAN & NEUMAIER COMPENSATED SUM     \n";
    std::cout << "========================================\n\n";

    // Test 1: Summing 1,000,000 tiny numbers (0.0001f) in 32-bit float
    // Exact mathematical value: 1,000,000 * 0.0001 = 100.0
    const int N = 1000000;
    std::vector<float> tinyValues(N, 0.0001f);

    float naiveF = numerical::naiveSum(tinyValues);
    float kahanF = numerical::kahanSum(tinyValues);
    float neumaierF = numerical::neumaierSum(tinyValues);

    // High-precision reference sum
    double refExact = 0.0;
    for (float x : tinyValues) refExact += static_cast<double>(x);

    std::cout << "Test 1: Summing 1,000,000 copies of 0.0001f (float32)\n";
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "  Double Exact Sum : " << refExact << "\n";
    std::cout << "  Naive float Sum  : " << naiveF << " (Error: " << std::abs(naiveF - refExact) << ")\n";
    std::cout << "  Kahan float Sum  : " << kahanF << " (Error: " << std::abs(kahanF - refExact) << ")\n";
    std::cout << "  Neumaier Sum     : " << neumaierF << " (Error: " << std::abs(neumaierF - refExact) << ")\n\n";

    // Test 2: Catastrophic Cancellation (Large + Tiny - Large)
    // Sequence: [1.0, 1e16, 1.0, -1e16] -> exact answer = 2.0
    // In IEEE-754 double (53-bit significand ~ 9e15), adding 1.0 to 1e16 drops 1.0 completely!
    std::vector<double> cancellation = {1.0, 1e16, 1.0, -1e16};

    double naiveD = numerical::naiveSum(cancellation);
    double kahanD = numerical::kahanSum(cancellation);
    double neumaierD = numerical::neumaierSum(cancellation);

    std::cout << "Test 2: Catastrophic Cancellation [1.0, 1e16, 1.0, -1e16] (float64)\n";
    std::cout << "  True Answer      : 2.000000\n";
    std::cout << "  Naive Sum        : " << naiveD << " (Completely loses small terms!)\n";
    std::cout << "  Kahan Sum        : " << kahanD << " (Fails when |x_i| > |sum|)\n";
    std::cout << "  Neumaier Sum     : " << neumaierD << " (Preserves exact 2.000000!)\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
