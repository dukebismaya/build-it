/**
 * ============================================================================
 * Sieve of Sundaram (Odd Prime Generation Algorithm)
 * ============================================================================
 * 
 * Description:
 * The Sieve of Sundaram (discovered in 1934 by Indian mathematician S. P. Sundaram)
 * is an alternative prime-generating sieve that directly identifies all odd
 * primes up to 2m + 2.
 * 
 * Mathematical Formulation:
 * Consider numbers k of the form:
 *   k = i + j + 2 * i * j
 * where 1 <= i <= j and i + j + 2*i*j <= m.
 * 
 * Why does this work?
 * Notice that:
 *   2 * k + 1 = 2 * (i + j + 2 * i * j) + 1
 *             = 2*i + 2*j + 4*i*j + 1
 *             = (2*i + 1) * (2*j + 1)
 * 
 * Since i >= 1 and j >= 1, we have (2*i + 1) >= 3 and (2*j + 1) >= 3.
 * Thus (2*i + 1) * (2*j + 1) is an odd composite number!
 * Every odd composite number factorizes into two odd integers >= 3, which can
 * be written as (2i + 1)(2j + 1).
 * Therefore, an odd integer 2k + 1 is prime IF AND ONLY IF k cannot be written
 * in the form i + j + 2*i*j.
 * 
 * Generating Primes up to N:
 * - Let m = (N - 1) / 2.
 * - Sieve out all k = i + j + 2*i*j <= m.
 * - For all uncrossed k in [1, m], 2*k + 1 is prime.
 * - Add 2 (the only even prime).
 * 
 * Time Complexity:
 * - O(m log m) or O(N log N)
 * - Space Complexity: O(N / 2) boolean array
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <algorithm>

namespace sundaram {

/**
 * Generates all prime numbers up to n using the Sieve of Sundaram.
 */
std::vector<int> generatePrimes(int n) {
    std::vector<int> primes;
    if (n < 2) return primes;

    // 2 is always prime
    primes.push_back(2);
    if (n == 2) return primes;

    int m = (n - 1) / 2;
    // marked[k] = true if k = i + j + 2*i*j (meaning 2k + 1 is composite)
    std::vector<bool> marked(m + 1, false);

    for (int i = 1; i <= m; ++i) {
        // Since j >= i, the minimum value for given i is i + i + 2*i*i = 2*i*(i + 1)
        int min_val = 2 * i * (i + 1);
        if (min_val > m) break;

        for (int j = i; ; ++j) {
            int k = i + j + 2 * i * j;
            if (k > m) break;
            marked[k] = true;
        }
    }

    // Collect all uncrossed numbers
    for (int k = 1; k <= m; ++k) {
        if (!marked[k]) {
            int p = 2 * k + 1;
            if (p <= n) {
                primes.push_back(p);
            }
        }
    }

    return primes;
}

/**
 * Standard trial division check for prime validation.
 */
bool isPrimeTrial(int x) {
    if (x < 2) return false;
    for (int d = 2; d * d <= x; ++d) {
        if (x % d == 0) return false;
    }
    return true;
}

} // namespace sundaram

int main() {
    std::cout << "========================================================\n";
    std::cout << " Sieve of Sundaram (Odd Prime Generator)\n";
    std::cout << "========================================================\n\n";

    constexpr int LIMIT = 100;
    std::cout << "Generating all primes up to " << LIMIT << " using Sieve of Sundaram:\n";

    auto primes = sundaram::generatePrimes(LIMIT);

    std::cout << "Found " << primes.size() << " primes <= " << LIMIT << ":\n";
    for (size_t i = 0; i < primes.size(); ++i) {
        std::cout << std::setw(4) << primes[i];
        if ((i + 1) % 10 == 0 || i + 1 == primes.size()) {
            std::cout << "\n";
        }
    }

    std::cout << "\n--------------------------------------------------------\n";
    std::cout << " Validation against Trial Division:\n";
    std::cout << "--------------------------------------------------------\n";

    bool all_valid = true;
    for (int p : primes) {
        if (!sundaram::isPrimeTrial(p)) {
            std::cout << "Error: " << p << " is not prime!\n";
            all_valid = false;
        }
    }

    int expected_count = 0;
    for (int i = 2; i <= LIMIT; ++i) {
        if (sundaram::isPrimeTrial(i)) {
            expected_count++;
        }
    }

    if (all_valid && static_cast<int>(primes.size()) == expected_count) {
        std::cout << "All " << primes.size() << " primes correctly generated and verified!\n";
    } else {
        std::cout << "Validation mismatch!\n";
    }

    // Larger benchmark check
    int large_limit = 10000;
    auto large_primes = sundaram::generatePrimes(large_limit);
    std::cout << "\nPrimes count up to " << large_limit << ": "
              << large_primes.size() << " (pi(10000) = 1229) -> "
              << (large_primes.size() == 1229 ? "EXACT MATCH!" : "MISMATCH!") << "\n";

    return 0;
}
