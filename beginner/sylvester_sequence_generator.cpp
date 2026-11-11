/**
 * @file sylvester_sequence_generator.cpp
 * @brief Sylvester's Sequence and Egyptian Fraction decomposition of 1 in C++17.
 *
 * Definition:
 *   Sylvester's sequence is an integer sequence in which each term is the product
 *   of all previous terms, plus one:
 *     a_0 = 2
 *     a_n = 1 + prod_{i=0}^{n-1} a_i  =  a_{n-1}^2 - a_{n-1} + 1
 *
 *   The sequence begins:
 *     2, 3, 7, 43, 1807, 3263443, 10650056950807, ...
 *
 * Mathematical Properties:
 *   1. Pairwise Coprimality: Every pair of distinct terms in Sylvester's sequence
 *      is coprime: gcd(a_i, a_j) = 1 for all i != j. (Provides a constructive proof
 *      that there are infinitely many prime numbers).
 *   2. Rapid Convergence to 1 (Greedy Egyptian Fraction):
 *      The sum of the reciprocals converges doubly-exponentially fast to 1:
 *        1/2 + 1/3 + 1/7 + 1/43 + 1/1807 + ... = 1
 *      Partial sum identity:
 *        sum_{i=0}^{n} (1 / a_i) = 1 - 1 / (a_{n+1} - 1).
 *
 * Complexity:
 *   - Time: O(n) arithmetic operations (terms grow doubly-exponentially, fitting in
 *     128-bit unsigned integers up to a_7).
 *   - Space: O(n).
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <cstdint>

namespace sequences {

using uint128 = unsigned __int128;

std::string toString128(uint128 val) {
    if (val == 0) return "0";
    std::string s;
    while (val > 0) {
        s += static_cast<char>('0' + (val % 10));
        val /= 10;
    }
    return std::string(s.rbegin(), s.rend());
}

/**
 * @brief Generates Sylvester's sequence terms a_0 through a_n.
 * @param n Max index (up to n = 7 fits in 128-bit unsigned integers).
 */
std::vector<uint128> generateSylvester(size_t n) {
    std::vector<uint128> seq;
    uint128 current = 2;
    seq.push_back(current);

    for (size_t i = 1; i <= n && i <= 7; ++i) {
        current = current * current - current + 1;
        seq.push_back(current);
    }
    return seq;
}

} // namespace sequences

int main() {
    std::cout << "========================================\n";
    std::cout << "     SYLVESTER'S SEQUENCE GENERATOR     \n";
    std::cout << "   (Egyptian Fraction Expansion of 1)   \n";
    std::cout << "========================================\n\n";

    auto seq = sequences::generateSylvester(7);

    std::cout << std::left << std::setw(4)  << "n"
              << std::setw(30) << "a_n"
              << std::setw(22) << "Partial Sum sum(1/a_i)"
              << "Remaining Gap (1 - Sum)\n";
    std::cout << std::string(80, '-') << "\n";

    double partialSum = 0.0;
    for (size_t i = 0; i < seq.size(); ++i) {
        partialSum += 1.0 / static_cast<double>(seq[i]);
        double gap = 1.0 - partialSum;

        std::cout << std::left << std::setw(4) << i
                  << std::setw(30) << sequences::toString128(seq[i])
                  << std::fixed << std::setprecision(15) << std::setw(22) << partialSum
                  << std::scientific << std::setprecision(4) << gap << "\n";
    }

    std::cout << "\nExact Fraction Identity Check (first 4 terms):\n";
    std::cout << "  1/2 + 1/3 + 1/7 + 1/43 = 1805 / 1806 = 1 - 1 / (1807 - 1)\n";
    std::cout << "  Notice (a_4 - 1) = " << sequences::toString128(seq[4] - 1) << ".\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
