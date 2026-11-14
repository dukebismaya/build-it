/**
 * @file jacobsthal_number_generator.cpp
 * @brief Jacobsthal and Jacobsthal-Lucas sequence generator and tiling in C++17.
 *
 * Mathematical Definitions:
 *   The Jacobsthal numbers J(n) are defined by the linear recurrence:
 *     J(0) = 0
 *     J(1) = 1
 *     J(n) = J(n - 1) + 2 * J(n - 2),  for n >= 2.
 *
 *   The sequence begins:
 *     0, 1, 1, 3, 5, 11, 21, 43, 85, 171, 341, 683, 1365, 2731, 5461, ...
 *
 * Closed Form:
 *     J(n) = (2^n - (-1)^n) / 3
 *
 * Complementary Sequence (Jacobsthal-Lucas numbers j(n)):
 *     j(0) = 2, j(1) = 1, j(n) = j(n - 1) + 2 * j(n - 2) = 2^n + (-1)^n.
 *     Sequence: 2, 1, 5, 7, 17, 31, 65, 127, 257, 511, ...
 *
 * Combinatorial Tiling Interpretation:
 *   J(n + 1) is the number of ways to tile a 2 x n rectangle with:
 *     - 1 x 2 dominoes (either horizontal or vertical), and
 *     - 2 x 2 squares.
 *
 * Complexity:
 *   - Iterative Generation: O(N) time, O(N) space.
 *   - Closed Form Direct Calculation: O(1) time.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>

namespace sequences {

/**
 * @brief Generates the first N terms of the Jacobsthal sequence J(0) to J(n).
 */
std::vector<uint64_t> generateJacobsthal(size_t n) {
    if (n == 0) return {0};
    std::vector<uint64_t> j(n + 1);
    j[0] = 0;
    j[1] = 1;
    for (size_t i = 2; i <= n; ++i) {
        j[i] = j[i - 1] + 2ULL * j[i - 2];
    }
    return j;
}

/**
 * @brief Generates the first N terms of the Jacobsthal-Lucas sequence j(0) to j(n).
 */
std::vector<uint64_t> generateJacobsthalLucas(size_t n) {
    if (n == 0) return {2};
    std::vector<uint64_t> jl(n + 1);
    jl[0] = 2;
    jl[1] = 1;
    for (size_t i = 2; i <= n; ++i) {
        jl[i] = jl[i - 1] + 2ULL * jl[i - 2];
    }
    return jl;
}

/**
 * @brief Computes Jacobsthal number directly via closed form (2^n - (-1)^n) / 3.
 */
uint64_t jacobsthalClosedForm(int n) {
    if (n < 0 || n > 62) return 0;
    int64_t negOnePow = (n % 2 == 0) ? 1 : -1;
    return ((1ULL << n) - negOnePow) / 3ULL;
}

} // namespace sequences

int main() {
    std::cout << "========================================\n";
    std::cout << "     JACOBSTHAL SEQUENCE GENERATOR      \n";
    std::cout << "    (Recurrence, Closed Form & Lucas)   \n";
    std::cout << "========================================\n\n";

    size_t limit = 15;
    auto j = sequences::generateJacobsthal(limit);
    auto jl = sequences::generateJacobsthalLucas(limit);

    std::cout << std::left << std::setw(6)  << "n"
              << std::setw(16) << "Jacobsthal J(n)"
              << std::setw(16) << "Closed Form"
              << std::setw(16) << "Lucas j(n)"
              << "2 x (n-1) Tilings\n";
    std::cout << std::string(72, '-') << "\n";

    for (size_t i = 0; i <= limit; ++i) {
        uint64_t direct = sequences::jacobsthalClosedForm(static_cast<int>(i));
        std::string tilings = (i > 0) ? std::to_string(j[i]) : "1 (Empty)";

        std::cout << std::left << std::setw(6)  << i
                  << std::setw(16) << j[i]
                  << std::setw(16) << direct
                  << std::setw(16) << jl[i]
                  << tilings << "\n";
    }

    std::cout << "\nAlgebraic Identity Check: j(n) * J(n) = J(2n):\n";
    for (size_t i = 1; i <= 6; ++i) {
        uint64_t prod = jl[i] * j[i];
        uint64_t j2n = j[2 * i];
        std::cout << "  n = " << i << ": j(" << i << ") * J(" << i << ") = "
                  << jl[i] << " * " << j[i] << " = " << prod
                  << " | J(" << 2 * i << ") = " << j2n
                  << " -> " << (prod == j2n ? "MATCH" : "FAIL") << "\n";
    }

    std::cout << "\nProgram finished successfully.\n";
    return 0;
}
