/**
 * @file perrin_number_checker.cpp
 * @brief Analysis of Perrin Numbers and the Perrin Primality Test in C++17.
 *
 * Definition:
 *   The Perrin sequence is defined by the third-order recurrence:
 *     P(0) = 3
 *     P(1) = 0
 *     P(2) = 2
 *     P(n) = P(n - 2) + P(n - 3)  for n >= 3.
 *
 * The sequence begins:
 *   3, 0, 2, 3, 2, 5, 5, 7, 10, 12, 17, 22, 29, 39, 51, 68, 90, 119, ...
 *
 * The Perrin Primality Criterion:
 *   - If p is a prime number, then p divides P(p).
 *     Examples:
 *       P(2) = 2   (2 | 2)
 *       P(3) = 3   (3 | 3)
 *       P(5) = 5   (5 | 5)
 *       P(7) = 7   (7 | 7)
 *       P(11) = 22 (11 | 22)
 *       P(13) = 39 (13 | 39)
 *   - A composite number n that satisfies n | P(n) is called a Perrin Pseudoprime.
 *     Perrin pseudoprimes are extremely rare; the smallest is 271441.
 *
 * Complexity:
 *   - Direct generation of first N numbers: O(N) time, O(N) space.
 *   - Divisibility test: O(1) per precomputed term.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>

namespace sequences {

/**
 * @brief Generates the first N terms of the Perrin sequence.
 */
std::vector<uint64_t> generatePerrinSequence(size_t n) {
    if (n == 0) return {3};
    if (n == 1) return {3, 0};
    if (n == 2) return {3, 0, 2};

    std::vector<uint64_t> p(n + 1);
    p[0] = 3;
    p[1] = 0;
    p[2] = 2;

    for (size_t i = 3; i <= n; ++i) {
        p[i] = p[i - 2] + p[i - 3];
    }
    return p;
}

/**
 * @brief Simple trial division primality test.
 */
bool isPrime(uint64_t n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (uint64_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

} // namespace sequences

int main() {
    std::cout << "========================================\n";
    std::cout << "        PERRIN NUMBER CHECKER           \n";
    std::cout << "   (Perrin Primality Property Test)     \n";
    std::cout << "========================================\n\n";

    size_t count = 25;
    auto perrin = sequences::generatePerrinSequence(count);

    std::cout << std::left << std::setw(6)  << "n"
              << std::setw(14) << "P(n)"
              << std::setw(14) << "P(n) mod n"
              << std::setw(14) << "n is Prime?"
              << "Property Holds?\n";
    std::cout << std::string(62, '-') << "\n";

    for (size_t n = 2; n <= count; ++n) {
        uint64_t rem = perrin[n] % n;
        bool prime = sequences::isPrime(n);
        bool divides = (rem == 0);

        std::cout << std::left << std::setw(6)  << n
                  << std::setw(14) << perrin[n]
                  << std::setw(14) << rem
                  << std::setw(14) << (prime ? "Yes" : "No")
                  << (divides ? (prime ? "PASS (Prime divides P(n))" : "PSEUDOPRIME!") : "PASS (Composite)") << "\n";
    }

    std::cout << "\nNotes:\n";
    std::cout << "  - For all primes p tested above, P(p) is an exact multiple of p.\n";
    std::cout << "  - The smallest Perrin pseudoprime is 271,441 = 521^2.\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
