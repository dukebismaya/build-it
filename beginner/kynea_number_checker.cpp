/**
 * @file kynea_number_checker.cpp
 * @brief Analysis and primality testing of Kynea Numbers in C++17.
 *
 * Mathematical Definition:
 *   A Kynea number is an integer of the form:
 *     K_n = (2^n + 1)^2 - 2  =  4^n + 2^(n + 1) - 1,  for n >= 1.
 *
 * The sequence begins:
 *   7, 23, 79, 287, 1087, 4223, 16639, 66047, 263167, ...
 *
 * Binary Representation Property:
 *   For n >= 1, the binary representation of K_n consists of:
 *     a leading 1, followed by (n - 1) zeros, followed by (n + 1) consecutive ones.
 *   Examples:
 *     - n = 1: 7    = 111_2
 *     - n = 2: 23   = 10111_2   (1 one, 1 zero, 3 ones)
 *     - n = 3: 79   = 1001111_2 (1 one, 2 zeros, 4 ones)
 *     - n = 4: 287  = 100011111_2
 *
 * Kynea Primes:
 *   Kynea numbers that are prime: 7, 23, 79, 1087, ...
 *
 * Complexity:
 *   - Generation: O(1) using 64-bit integer bit shifts.
 *   - Primality test: O(sqrt(K_n)) trial division.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <string>

namespace number_theory {

/**
 * @brief Computes nth Kynea number K_n = (2^n + 1)^2 - 2 for n in [1, 31].
 */
uint64_t computeKynea(int n) {
    if (n < 1 || n > 31) return 0;
    uint64_t term = (1ULL << n) + 1ULL;
    return term * term - 2ULL;
}

/**
 * @brief Trial division primality test.
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

std::string toBinary(uint64_t val) {
    if (val == 0) return "0";
    std::string s;
    while (val > 0) {
        s += static_cast<char>('0' + (val & 1));
        val >>= 1;
    }
    return std::string(s.rbegin(), s.rend());
}

} // namespace number_theory

int main() {
    std::cout << "========================================\n";
    std::cout << "         KYNEA NUMBER CHECKER           \n";
    std::cout << "    (K_n = (2^n + 1)^2 - 2 Analysis)    \n";
    std::cout << "========================================\n\n";

    std::cout << std::left << std::setw(6)  << "n"
              << std::setw(16) << "Kynea K_n"
              << std::setw(24) << "Binary Representation"
              << "Is Kynea Prime?\n";
    std::cout << std::string(64, '-') << "\n";

    for (int n = 1; n <= 11; ++n) {
        uint64_t k = number_theory::computeKynea(n);
        bool prime = number_theory::isPrime(k);
        std::string binStr = number_theory::toBinary(k);

        std::cout << std::left << std::setw(6)  << n
                  << std::setw(16) << k
                  << std::setw(24) << binStr
                  << (prime ? "PRIME (Kynea Prime)" : "Composite") << "\n";
    }

    std::cout << "\nBinary Pattern Verification:\n";
    std::cout << "  - Notice the structure: 1 followed by (n-1) zeros, then (n+1) ones.\n";
    std::cout << "  - Paired with Carol numbers: K_n - C_n = 2^(n+2).\n";
    std::cout << "    Example (n=3): K_3 - C_3 = 79 - 47 = 32 = 2^5.\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
