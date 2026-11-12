/**
 * @file golomb_sequence_generator.cpp
 * @brief Golomb's self-describing sequence generator and verification in C++17.
 *
 * Definition:
 *   Golomb's sequence G(n) (also known as Silverman's sequence) is the unique
 *   non-decreasing sequence of positive integers where G(n) is the number of times
 *   n appears in the sequence.
 *
 * Base values and recurrence:
 *   G(1) = 1
 *   G(n + 1) = 1 + G(n + 1 - G(G(n)))  for n >= 1.
 *
 * The sequence begins:
 *   n:    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, ...
 *   G(n): 1, 2, 2, 3, 3, 4, 4, 4, 5,  5,  5,  6, ...
 *
 * Interpretation:
 *   - '1' appears G(1) = 1 time: [1]
 *   - '2' appears G(2) = 2 times: [2, 2]
 *   - '3' appears G(3) = 2 times: [3, 3]
 *   - '4' appears G(4) = 3 times: [4, 4, 4]
 *   - '5' appears G(5) = 3 times: [5, 5, 5]
 *
 * Asymptotic Growth:
 *   G(n) ~ phi^(2 - phi) * n^(phi - 1)  where phi = (1 + sqrt(5)) / 2 ~= 1.6180339887.
 *   Exponent (phi - 1) ~= 0.6180339887.
 *
 * Complexity:
 *   - Time: O(N) to generate the first N terms.
 *   - Space: O(N) sequence storage.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <unordered_map>

namespace sequences {

/**
 * @brief Generates the first N terms of Golomb's sequence G(1) to G(n).
 */
std::vector<int> generateGolomb(int n) {
    if (n <= 0) return {};
    std::vector<int> g(n + 1, 0);
    g[1] = 1;

    for (int i = 1; i < n; ++i) {
        g[i + 1] = 1 + g[i + 1 - g[g[i]]];
    }
    return g;
}

/**
 * @brief Asymptotic estimate of G(n) using the golden ratio phi.
 */
double asymptoticGolomb(int n) {
    const double phi = (1.0 + std::sqrt(5.0)) / 2.0;
    double c = std::pow(phi, 2.0 - phi);
    return c * std::pow(static_cast<double>(n), phi - 1.0);
}

} // namespace sequences

int main() {
    std::cout << "========================================\n";
    std::cout << "      GOLOMB'S SEQUENCE GENERATOR       \n";
    std::cout << "   (Self-Describing Integer Sequence)   \n";
    std::cout << "========================================\n\n";

    int limit = 30;
    auto g = sequences::generateGolomb(limit);

    std::cout << std::left << std::setw(6)  << "n"
              << std::setw(10) << "G(n)"
              << std::setw(16) << "Asymptotic Est"
              << "Relative Error (%)\n";
    std::cout << std::string(50, '-') << "\n";

    std::unordered_map<int, int> counts;

    for (int i = 1; i <= limit; ++i) {
        counts[g[i]]++;
        double est = sequences::asymptoticGolomb(i);
        double err = std::abs(g[i] - est) / g[i] * 100.0;

        std::cout << std::left << std::setw(6) << i
                  << std::setw(10) << g[i]
                  << std::fixed << std::setprecision(4) << std::setw(16) << est
                  << std::setprecision(2) << err << "%\n";
    }

    std::cout << "\nSelf-Describing Frequency Verification:\n";
    std::cout << "Checking that each value k appears exactly G(k) times:\n";
    for (int k = 1; k <= 5; ++k) {
        std::cout << "  Value " << k << ": Appears " << counts[k] << " times | Expected G("
                  << k << ") = " << g[k]
                  << " -> " << (counts[k] == g[k] ? "MATCH" : "MISMATCH") << "\n";
    }

    std::cout << "\nProgram finished successfully.\n";
    return 0;
}
