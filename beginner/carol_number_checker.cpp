/**
 * @file carol_number_checker.cpp
 * @brief Analysis and primality testing of Carol Numbers in C++17.
 *
 * Mathematical Definition:
 *   A Carol number is an integer of the form:
 *     C_n = (2^n - 1)^2 - 2  =  4^n - 2^(n + 1) - 1,  for n >= 2.
 *
 * The sequence begins:
 *   7, 47, 223, 959, 3967, 16127, 65023, 261119, 1046527, ...
 *
 * Binary Representation Property:
 *   For n >= 2, the binary representation of C_n consists of:
 *     (n - 1) consecutive ones, followed by a single zero, followed by (n + 1) consecutive ones.
 *   Examples:
 *     - n = 2: 7     = 111_2 = (1 one, 0 zeros?? For n=2: 4 - 4 - 1 ... binary 111)
 *     - n = 3: 47    = 101111_2   (2 ones, 1 zero, 3 ones)
 *     - n = 4: 223   = 11011111_2 (3 ones, 1 zero, 4 ones)
 *
 * Carol Primes:
 *   Carol numbers that are prime: 7, 47, 223, 3967, 16127, ...
 *
 * Complexity:
 *   - Generation: O(1) using 64-bit integer bit shifts.
 *   - Primality test: O(sqrt(C_n)) trial division.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <string>
#include <bitset>

namespace number_theory {

/**
 * @brief Computes nth Carol number C_n = (2^n - 1)^2 - 2 for n in [2, 31].
 */
uint64_t computeCarol(int n) {
    if (n < 2 || n > 31) return 0;
    uint64_t term = (1ULL << n) - 1ULL;
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
    std::cout << "         CAROL NUMBER CHECKER           \n";
    std::cout << "    (C_n = (2^n - 1)^2 - 2 Analysis)    \n";
    std::cout << "========================================\n\n";

    std::cout << std::left << std::setw(6)  << "n"
              << std::setw(16) << "Carol C_n"
              << std::setw(24) << "Binary Representation"
              << "Is Carol Prime?\n";
    std::cout << std::string(64, '-') << "\n";

    for (int n = 2; n <= 12; ++n) {
        uint64_t c = number_theory::computeCarol(n);
        bool prime = number_theory::isPrime(c);
        std::string binStr = number_theory::toBinary(c);

        std::cout << std::left << std::setw(6)  << n
                  << std::setw(16) << c
                  << std::setw(24) << binStr
                  << (prime ? "PRIME (Carol Prime)" : "Composite") << "\n";
    }

    std::cout << "\nNotes on Binary Structure:\n";
    std::cout << "  - For n = 3: 47  = 101111_2 (2 ones, 1 zero, 4 ones - wait, 101111)\n";
    std::cout << "  - For n = 4: 223 = 11011111_2 (3 ones, 1 zero, 5 ones)\n";
    std::cout << "  - Notice the characteristic lone zero bit at position n!\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
