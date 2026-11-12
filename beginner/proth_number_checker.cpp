/**
 * @file proth_number_checker.cpp
 * @brief Proth Number identifier and Proth's Primality Test in C++17.
 *
 * Mathematical Definition:
 *   A Proth number is an integer of the form:
 *     N = k * 2^n + 1
 *   where k is an odd positive integer, n is a positive integer, and 2^n > k.
 *
 * The sequence begins:
 *   3, 5, 9, 13, 17, 25, 33, 41, 49, 57, 65, 81, 97, 113, ...
 *
 * Proth's Theorem (1878):
 *   A Proth number N is prime if and only if there exists an integer a such that:
 *     a^((N - 1) / 2) == -1 (mod N)  [i.e. == N - 1 (mod N)]
 *
 * Practical Application:
 *   Testing a = 3 (or 5, 7 if 3 divides N) yields a deterministic primality test
 *   without requiring full prime factorization of (N - 1).
 *
 * Complexity:
 *   - Proth decomposition: O(log N) trial divisions by 2.
 *   - Primality test via modular exponentiation: O(log^2 N * log log N).
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>
#include <optional>

namespace number_theory {

using uint128 = unsigned __int128;

/**
 * @brief Computes (base^exp) mod mod using 128-bit unsigned integers.
 */
uint64_t powerMod(uint64_t base, uint64_t exp, uint64_t mod) {
    uint128 res = 1;
    uint128 b = base % mod;
    while (exp > 0) {
        if (exp & 1) {
            res = (res * b) % mod;
        }
        b = (b * b) % mod;
        exp >>= 1;
    }
    return static_cast<uint64_t>(res);
}

struct ProthDecomposition {
    uint64_t k;
    int n;
};

/**
 * @brief Checks if N is a valid Proth number N = k * 2^n + 1 with k odd and 2^n > k.
 */
std::optional<ProthDecomposition> decomposeProth(uint64_t N) {
    if (N < 3 || (N % 2 == 0)) return std::nullopt;

    uint64_t m = N - 1; // k * 2^n
    int n = 0;
    while (m > 0 && (m % 2 == 0)) {
        m /= 2;
        n++;
    }
    uint64_t k = m; // Remaining odd factor

    // Condition: k must be odd, and 2^n > k
    if (k % 2 != 1) return std::nullopt;
    if (n >= 63 || (1ULL << n) > k) {
        return ProthDecomposition{k, n};
    }
    return std::nullopt;
}

/**
 * @brief Tests if a Proth number is prime using Proth's Theorem.
 * @param N The Proth number.
 * @return True if N is prime, false otherwise.
 */
bool prothPrimalityTest(uint64_t N) {
    if (N <= 1) return false;
    if (N == 3 || N == 5) return true;

    auto decomp = decomposeProth(N);
    if (!decomp) return false;

    uint64_t exp = (N - 1) / 2;

    // Test small bases a = 2, 3, 5, 7
    std::vector<uint64_t> bases = {3, 5, 7, 2};
    for (uint64_t a : bases) {
        if (a >= N) continue;
        uint64_t res = powerMod(a, exp, N);
        if (res == N - 1) {
            return true; // Proth's condition satisfied!
        }
    }

    return false;
}

} // namespace number_theory

int main() {
    std::cout << "========================================\n";
    std::cout << "        PROTH NUMBER CHECKER            \n";
    std::cout << "      (Proth's Primality Test)          \n";
    std::cout << "========================================\n\n";

    std::vector<uint64_t> testNumbers = {3, 5, 9, 13, 17, 25, 33, 41, 49, 57, 65, 97, 113, 129};

    std::cout << std::left << std::setw(8)  << "N"
              << std::setw(20) << "Decomposition"
              << std::setw(16) << "Is Proth Form?"
              << "Proth Primality Test\n";
    std::cout << std::string(66, '-') << "\n";

    for (uint64_t num : testNumbers) {
        auto decomp = number_theory::decomposeProth(num);
        if (decomp) {
            std::string form = std::to_string(decomp->k) + " * 2^" + std::to_string(decomp->n) + " + 1";
            bool isPrime = number_theory::prothPrimalityTest(num);

            std::cout << std::left << std::setw(8)  << num
                      << std::setw(20) << form
                      << std::setw(16) << "YES (2^n > k)"
                      << (isPrime ? "PRIME (Proth Prime)" : "Composite") << "\n";
        } else {
            std::cout << std::left << std::setw(8)  << num
                      << std::setw(20) << "-"
                      << std::setw(16) << "NO"
                      << "N/A\n";
        }
    }

    std::cout << "\nNotes on Proth Primes:\n";
    std::cout << "  - Fermat primes (k = 1): F_0 = 3, F_1 = 5, F_2 = 17 are Proth primes.\n";
    std::cout << "  - Cullen primes (k = n): C_1 = 1*2^1 + 1 = 3 is a Proth prime.\n";
    std::cout << "  - Largest known primes that are not Mersenne primes are typically Proth primes.\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
