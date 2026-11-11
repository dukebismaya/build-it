/**
 * @file padovan_sequence_generator.cpp
 * @brief Padovan sequence generation and Plastic Number convergence in C++17.
 *
 * Definition:
 *   The Padovan sequence is an integer sequence defined by:
 *     P(0) = P(1) = P(2) = 1
 *     P(n) = P(n - 2) + P(n - 3)  for n >= 3.
 *
 *   The sequence begins:
 *     1, 1, 1, 2, 2, 3, 4, 5, 7, 9, 12, 16, 21, 28, 37, 49, 65, ...
 *
 * Plastic Number Connection:
 *   Just as consecutive ratios of Fibonacci numbers converge to the Golden Ratio
 *   phi ~= 1.6180339887, consecutive ratios of Padovan numbers P(n) / P(n - 1)
 *   converge to the Plastic Constant (Plastic Number) rho:
 *     rho = cbrt(1/2 + sqrt(23/108)) + cbrt(1/2 - sqrt(23/108)) ~= 1.324717957244746
 *   which is the unique real root of x^3 - x - 1 = 0.
 *
 * Complexity:
 *   - Time: O(N) to generate first N terms.
 *   - Space: O(N) sequence storage (or O(1) iterative space).
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <cstdint>

namespace sequences {

/**
 * @brief Generates Padovan sequence terms P(0) through P(n).
 */
std::vector<uint64_t> generatePadovan(size_t n) {
    if (n == 0) return {1};
    if (n == 1) return {1, 1};
    if (n == 2) return {1, 1, 1};

    std::vector<uint64_t> p(n + 1);
    p[0] = 1;
    p[1] = 1;
    p[2] = 1;

    for (size_t i = 3; i <= n; ++i) {
        p[i] = p[i - 2] + p[i - 3];
    }
    return p;
}

/**
 * @brief Computes exact Plastic Constant (real root of x^3 - x - 1 = 0).
 */
double computePlasticConstant() {
    double inner = std::sqrt(23.0 / 108.0);
    return std::cbrt(0.5 + inner) + std::cbrt(0.5 - inner);
}

} // namespace sequences

int main() {
    std::cout << "========================================\n";
    std::cout << "      PADOVAN SEQUENCE GENERATOR        \n";
    std::cout << "   (Plastic Number Ratio Convergence)   \n";
    std::cout << "========================================\n\n";

    size_t limit = 30;
    auto padovan = sequences::generatePadovan(limit);
    double plastic = sequences::computePlasticConstant();

    std::cout << "Theoretical Plastic Constant (rho): " << std::fixed << std::setprecision(12)
              << plastic << "\n\n";

    std::cout << std::left << std::setw(6)  << "n"
              << std::setw(14) << "P(n)"
              << std::setw(20) << "Ratio P(n)/P(n-1)"
              << "Error |ratio - rho|\n";
    std::cout << std::string(62, '-') << "\n";

    for (size_t i = 0; i <= limit; ++i) {
        if (i == 0) {
            std::cout << std::left << std::setw(6) << i
                      << std::setw(14) << padovan[i]
                      << std::setw(20) << "-"
                      << "-\n";
        } else {
            double ratio = static_cast<double>(padovan[i]) / static_cast<double>(padovan[i - 1]);
            double err = std::abs(ratio - plastic);
            std::cout << std::left << std::setw(6)  << i
                      << std::setw(14) << padovan[i]
                      << std::setw(20) << std::setprecision(10) << ratio
                      << std::scientific << std::setprecision(3) << err << std::fixed << "\n";
        }
    }

    std::cout << "\nProgram finished successfully.\n";
    return 0;
}
