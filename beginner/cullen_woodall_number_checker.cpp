/**
 * @file cullen_woodall_number_checker.cpp
 * @brief Analysis and verification of Cullen and Woodall numbers in C++17.
 *
 * Mathematical Definitions:
 *   - A Cullen number C_n is an integer of the form:
 *       C_n = n * 2^n + 1,  for n >= 1.
 *     The sequence begins: 3, 9, 25, 65, 161, 385, 897, 2049, 4609, ...
 *   - A Woodall number W_n (or Cullen number of the second kind) is:
 *       W_n = n * 2^n - 1,  for n >= 1.
 *     The sequence begins: 1, 7, 23, 63, 159, 383, 895, 2047, 4607, ...
 *
 * Properties:
 *   - Both sequences grow exponentially.
 *   - Cullen primes: C_1 = 3, C_141, C_4713, ... (almost all Cullen numbers are composite).
 *   - Woodall primes: W_2 = 7, W_3 = 23, W_6 = 383, W_30, ...
 *   - For n > 1, if p = 8k - 1 is prime, then p divides W_{(p+1)/2}.
 *
 * Complexity:
 *   - Generation of C_n and W_n: O(1) using bit-shifts for n <= 58.
 *   - Inverse test (determine n given x): O(log(x)) via binary search.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <optional>

namespace number_theory {

/**
 * @brief Computes the nth Cullen number C_n = n * 2^n + 1.
 * @param n Index (1 <= n <= 58 to fit in uint64_t).
 * @return Cullen number C_n.
 */
uint64_t computeCullen(int n) {
    if (n < 1 || n > 58) return 0;
    return (static_cast<uint64_t>(n) << n) + 1ULL;
}

/**
 * @brief Computes the nth Woodall number W_n = n * 2^n - 1.
 * @param n Index (1 <= n <= 58 to fit in uint64_t).
 * @return Woodall number W_n.
 */
uint64_t computeWoodall(int n) {
    if (n < 1 || n > 58) return 0;
    return (static_cast<uint64_t>(n) << n) - 1ULL;
}

/**
 * @brief Checks if an integer is prime via trial division.
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

/**
 * @brief Determines if a number is a Cullen number, returning n if true.
 */
std::optional<int> findCullenIndex(uint64_t val) {
    if (val < 3) return std::nullopt;
    for (int n = 1; n <= 58; ++n) {
        uint64_t c = computeCullen(n);
        if (c == val) return n;
        if (c > val) break;
    }
    return std::nullopt;
}

/**
 * @brief Determines if a number is a Woodall number, returning n if true.
 */
std::optional<int> findWoodallIndex(uint64_t val) {
    if (val < 1) return std::nullopt;
    for (int n = 1; n <= 58; ++n) {
        uint64_t w = computeWoodall(n);
        if (w == val) return n;
        if (w > val) break;
    }
    return std::nullopt;
}

} // namespace number_theory

int main() {
    std::cout << "========================================\n";
    std::cout << "    CULLEN & WOODALL NUMBERS CHECKER    \n";
    std::cout << "========================================\n\n";

    std::cout << "First 10 Cullen & Woodall Numbers (n = 1 to 10):\n";
    std::cout << std::left << std::setw(6)  << "n"
              << std::setw(18) << "Cullen C_n"
              << std::setw(14) << "C_n Prime?"
              << std::setw(18) << "Woodall W_n"
              << "W_n Prime?\n";
    std::cout << std::string(68, '-') << "\n";

    for (int n = 1; n <= 10; ++n) {
        uint64_t c = number_theory::computeCullen(n);
        uint64_t w = number_theory::computeWoodall(n);
        bool cPrime = number_theory::isPrime(c);
        bool wPrime = number_theory::isPrime(w);

        std::cout << std::left << std::setw(6)  << n
                  << std::setw(18) << c
                  << std::setw(14) << (cPrime ? "PRIME" : "Composite")
                  << std::setw(18) << w
                  << (wPrime ? "PRIME" : "Composite") << "\n";
    }

    std::cout << "\nInverse Lookup Test Cases:\n";
    std::vector<uint64_t> testNumbers = {7, 9, 23, 25, 63, 65, 383, 385, 2026, 4609};

    for (uint64_t num : testNumbers) {
        auto cIdx = number_theory::findCullenIndex(num);
        auto wIdx = number_theory::findWoodallIndex(num);

        std::cout << "Value " << std::setw(5) << num << ": ";
        if (cIdx) {
            std::cout << "Cullen number C_" << *cIdx << " (n=" << *cIdx << "*2^" << *cIdx << " + 1) ";
        }
        if (wIdx) {
            std::cout << "Woodall number W_" << *wIdx << " (n=" << *wIdx << "*2^" << *wIdx << " - 1)";
        }
        if (!cIdx && !wIdx) {
            std::cout << "Neither Cullen nor Woodall";
        }
        std::cout << "\n";
    }

    std::cout << "\nProgram finished successfully.\n";
    return 0;
}
