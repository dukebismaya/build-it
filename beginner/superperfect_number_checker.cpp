/**
 * @file superperfect_number_checker.cpp
 * @brief Analysis and verification of Superperfect Numbers in C++17.
 *
 * Definition:
 *   A positive integer n is a superperfect number if:
 *     sigma(sigma(n)) = 2 * n
 *   where sigma(m) denotes the sum-of-divisors function (including 1 and m).
 *
 * Suryanarayana-Kanold Theorem (1969):
 *   An even integer n is superperfect if and only if:
 *     n = 2^k  and  (2^(k + 1) - 1) is a Mersenne prime.
 *
 * Known Even Superperfect Numbers:
 *   - k = 1:  n = 2^1  = 2       (sigma(2) = 3,    sigma(3) = 4 = 2*2)
 *   - k = 2:  n = 2^2  = 4       (sigma(4) = 7,    sigma(7) = 8 = 2*4)
 *   - k = 4:  n = 2^4  = 16      (sigma(16) = 31,  sigma(31) = 32 = 2*16)
 *   - k = 6:  n = 2^6  = 64      (sigma(64) = 127, sigma(127) = 128 = 2*64)
 *   - k = 12: n = 2^12 = 4096    (sigma(4096) = 8191, sigma(8191) = 8192 = 2*4096)
 *   - k = 16: n = 2^16 = 65536
 *
 * Open Problem:
 *   Whether any odd superperfect numbers exist is an unsolved problem (none exist < 10^24).
 *
 * Complexity:
 *   - sigma(n) computation via prime factorization: O(sqrt(n)).
 *   - Superperfect check: O(sqrt(n) + sqrt(sigma(n))).
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>

namespace number_theory {

/**
 * @brief Computes sigma(n), the sum of all positive divisors of n.
 */
uint64_t computeSigma(uint64_t n) {
    if (n == 0) return 0;
    if (n == 1) return 1;

    uint64_t sum = 1;
    uint64_t temp = n;

    for (uint64_t p = 2; p * p <= temp; ++p) {
        if (temp % p == 0) {
            uint64_t pPow = 1;
            uint64_t sumPowers = 1;
            while (temp % p == 0) {
                pPow *= p;
                sumPowers += pPow;
                temp /= p;
            }
            sum *= sumPowers;
        }
    }
    if (temp > 1) {
        sum *= (1 + temp);
    }
    return sum;
}

/**
 * @brief Checks if n is a superperfect number: sigma(sigma(n)) == 2*n.
 */
bool isSuperperfect(uint64_t n, uint64_t& sigma1, uint64_t& sigma2) {
    sigma1 = computeSigma(n);
    sigma2 = computeSigma(sigma1);
    return (sigma2 == 2 * n);
}

} // namespace number_theory

int main() {
    std::cout << "========================================\n";
    std::cout << "      SUPERPERFECT NUMBER CHECKER       \n";
    std::cout << "   (sigma(sigma(n)) == 2*n Analysis)    \n";
    std::cout << "========================================\n\n";

    // Test cases: powers of 2 and non-powers
    std::vector<uint64_t> testCandidates = {2, 4, 6, 8, 16, 28, 32, 64, 128, 4096};

    std::cout << std::left << std::setw(8)  << "n"
              << std::setw(12) << "sigma(n)"
              << std::setw(16) << "sigma(sigma(n))"
              << std::setw(10) << "2 * n"
              << "Is Superperfect?\n";
    std::cout << std::string(62, '-') << "\n";

    for (uint64_t n : testCandidates) {
        uint64_t s1 = 0, s2 = 0;
        bool super = number_theory::isSuperperfect(n, s1, s2);

        std::cout << std::left << std::setw(8)  << n
                  << std::setw(12) << s1
                  << std::setw(16) << s2
                  << std::setw(10) << (2 * n)
                  << (super ? "YES (Superperfect)" : "No") << "\n";
    }

    std::cout << "\nSuryanarayana Connection: Even superperfect numbers n = 2^k:\n";
    std::cout << "  - k = 1:  n = 2     -> Mersenne prime 2^(1+1) - 1 = 3\n";
    std::cout << "  - k = 2:  n = 4     -> Mersenne prime 2^(2+1) - 1 = 7\n";
    std::cout << "  - k = 4:  n = 16    -> Mersenne prime 2^(4+1) - 1 = 31\n";
    std::cout << "  - k = 6:  n = 64    -> Mersenne prime 2^(6+1) - 1 = 127\n";
    std::cout << "  - k = 12: n = 4096  -> Mersenne prime 2^(12+1) - 1 = 8191\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
