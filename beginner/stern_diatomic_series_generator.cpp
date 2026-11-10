/**
 * @file stern_diatomic_series_generator.cpp
 * @brief Stern's Diatomic Series and Calkin-Wilf Rational Sequence in C++17.
 *
 * Stern's diatomic series (also known as the Stern-Brocot sequence or fusc function)
 * is defined by the recurrence:
 *   s(0) = 0, s(1) = 1
 *   For n >= 1:
 *     s(2n)     = s(n)
 *     s(2n + 1) = s(n) + s(n + 1)
 *
 * The sequence begins:
 *   0, 1, 1, 2, 1, 3, 2, 3, 1, 4, 3, 5, 2, 5, 3, 4, 1, ...
 *
 * Remarkable Properties:
 *   1. Coprimality: Consecutive terms are always coprime: gcd(s(n), s(n+1)) = 1.
 *   2. Calkin-Wilf Tree: The sequence of irreducible fractions s(n) / s(n+1) for n >= 1
 *      enumerates EVERY positive rational number exactly once without repetition:
 *        1/1, 1/2, 2/1, 1/3, 3/2, 2/3, 3/1, 1/4, 4/3, 3/5, 5/2, ...
 *   3. Hyperbinary representation: s(n + 1) is the number of ways to write n as a sum of
 *      powers of 2, where each power of 2 is used at most twice.
 *
 * Complexity:
 *   - Direct generation of first N terms: O(N) time, O(N) space.
 *   - Fast single term s(n) computation: O(log(n)) time via binary state iteration.
 */

#include <iostream>
#include <vector>
#include <numeric>
#include <iomanip>
#include <cstdint>

namespace sequences {

/**
 * @brief Computes the first N terms of Stern's diatomic series in O(N).
 */
std::vector<uint64_t> generateSternSequence(size_t n) {
    if (n == 0) return {0};
    std::vector<uint64_t> s(n + 1, 0);
    s[0] = 0;
    if (n >= 1) s[1] = 1;

    for (size_t i = 1; 2 * i <= n; ++i) {
        s[2 * i] = s[i];
        if (2 * i + 1 <= n) {
            s[2 * i + 1] = s[i] + s[i + 1];
        }
    }
    return s;
}

/**
 * @brief Computes s(n) in O(log n) time without full sequence storage.
 */
uint64_t computeSternFast(uint64_t n) {
    uint64_t a = 1;
    uint64_t b = 0;
    while (n > 0) {
        if (n & 1) {
            b += a;
        } else {
            a += b;
        }
        n >>= 1;
    }
    return b;
}

} // namespace sequences

int main() {
    std::cout << "========================================\n";
    std::cout << "     STERN'S DIATOMIC SERIES GENERATOR  \n";
    std::cout << "========================================\n\n";

    size_t count = 20;
    auto stern = sequences::generateSternSequence(count);

    std::cout << "First " << count + 1 << " terms of Stern's diatomic series:\n";
    std::cout << std::left << std::setw(6)  << "n"
              << std::setw(10) << "s(n)"
              << std::setw(14) << "s(n) Fast"
              << std::setw(14) << "s(n)/s(n+1)"
              << "gcd(s(n), s(n+1))\n";
    std::cout << std::string(60, '-') << "\n";

    for (size_t i = 0; i < count; ++i) {
        uint64_t fast = sequences::computeSternFast(i);
        uint64_t g = std::gcd(stern[i], stern[i + 1]);
        std::string frac = std::to_string(stern[i]) + "/" + std::to_string(stern[i + 1]);

        std::cout << std::left << std::setw(6)  << i
                  << std::setw(10) << stern[i]
                  << std::setw(14) << fast
                  << std::setw(14) << frac
                  << g << "\n";
    }

    std::cout << "\nEnumeration of Rationals (Calkin-Wilf fractions s(n)/s(n+1) for n=1..15):\n";
    for (size_t i = 1; i <= 15; ++i) {
        std::cout << stern[i] << "/" << stern[i + 1] << (i == 15 ? "" : ", ");
    }
    std::cout << "\n\nProgram finished successfully.\n";
    return 0;
}
