/**
 * @file triangular_square_numbers.cpp
 * @brief Generation and Pell-Euler verification of Square Triangular Numbers in C++17.
 *
 * Mathematical Definition:
 *   A Square Triangular Number is a positive integer N that is simultaneously
 *   a perfect square (N = s^2) and a triangular number (N = t*(t + 1) / 2).
 *
 * The sequence begins:
 *   0, 1, 36, 1225, 41616, 1413721, 48024900, ...
 *
 * Euler's Recurrence Relations (via Pell's Equation x^2 - 8*y^2 = 1):
 *   - Square-Triangular values N_k:
 *       N_0 = 0, N_1 = 1
 *       N_k = 34 * N_{k-1} - N_{k-2} + 2
 *   - Square roots s_k (where N_k = s_k^2):
 *       s_0 = 0, s_1 = 1
 *       s_k = 6 * s_{k-1} - s_{k-2}
 *   - Triangular indices t_k (where N_k = t_k * (t_k + 1) / 2):
 *       t_0 = 0, t_1 = 1
 *       t_k = 6 * t_{k-1} - t_{k-2} + 2
 *
 * Complexity:
 *   - Time: O(K) to generate the first K square triangular numbers.
 *   - Space: O(1) auxiliary space.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>

namespace number_theory {

struct SquareTriangularEntry {
    int index;
    uint64_t value;
    uint64_t squareRoot;
    uint64_t triangularIndex;
};

/**
 * @brief Generates the first count Square Triangular numbers using Euler's recurrences.
 */
std::vector<SquareTriangularEntry> generateSquareTriangular(int count) {
    std::vector<SquareTriangularEntry> results;
    if (count <= 0) return results;

    uint64_t nPrev2 = 0, nPrev1 = 1;
    uint64_t sPrev2 = 0, sPrev1 = 1;
    uint64_t tPrev2 = 0, tPrev1 = 1;

    results.push_back({0, 0, 0, 0});
    if (count == 1) return results;

    results.push_back({1, 1, 1, 1});
    for (int k = 2; k < count; ++k) {
        uint64_t nCurr = 34 * nPrev1 - nPrev2 + 2;
        uint64_t sCurr = 6 * sPrev1 - sPrev2;
        uint64_t tCurr = 6 * tPrev1 - tPrev2 + 2;

        results.push_back({k, nCurr, sCurr, tCurr});

        nPrev2 = nPrev1; nPrev1 = nCurr;
        sPrev2 = sPrev1; sPrev1 = sCurr;
        tPrev2 = tPrev1; tPrev1 = tCurr;
    }

    return results;
}

} // namespace number_theory

int main() {
    std::cout << "========================================\n";
    std::cout << "     SQUARE TRIANGULAR NUMBERS          \n";
    std::cout << "   (Euler Recurrence & Verification)    \n";
    std::cout << "========================================\n\n";

    int numTerms = 11; // k = 0 through 10 fits comfortably in uint64_t
    auto entries = number_theory::generateSquareTriangular(numTerms);

    std::cout << std::left << std::setw(4)  << "k"
              << std::setw(22) << "N_k"
              << std::setw(14) << "Square (s_k)"
              << std::setw(14) << "Triang (t_k)"
              << "Identity Check\n";
    std::cout << std::string(70, '-') << "\n";

    for (const auto& e : entries) {
        bool sqValid = (e.squareRoot * e.squareRoot == e.value);
        bool triValid = (e.triangularIndex * (e.triangularIndex + 1) / 2 == e.value);

        std::cout << std::left << std::setw(4)  << e.index
                  << std::setw(22) << e.value
                  << std::setw(14) << e.squareRoot
                  << std::setw(14) << e.triangularIndex
                  << ((sqValid && triValid) ? "PASS (s^2 == t(t+1)/2)" : "FAIL") << "\n";
    }

    std::cout << "\nExample Verification for k = 2:\n";
    std::cout << "  N_2 = 36 = 6^2 = (8 * 9) / 2.\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
