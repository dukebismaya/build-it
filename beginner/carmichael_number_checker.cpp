/**
 * ============================================================================
 * Carmichael Number Checker & Korselt's Criterion Verification
 * ============================================================================
 * 
 * Description:
 * A Carmichael number is a composite positive integer n that satisfies Fermat's
 * Little Theorem congruence for all integers a that are coprime to n:
 *   a^(n - 1) = 1 (mod n)  for all a such that gcd(a, n) = 1.
 * 
 * Korselt's Criterion (1899):
 * A composite integer n is a Carmichael number if and only if:
 *   1. n is composite.
 *   2. n is square-free (no prime factor divides n more than once).
 *   3. For every prime divisor p of n, (p - 1) divides (n - 1):
 *        (p - 1) | (n - 1).
 * 
 * Examples:
 * - 561 = 3 * 11 * 17 (the smallest Carmichael number)
 *     p=3:  (3-1) = 2,  and 2 | 560 (560 / 2 = 280)
 *     p=11: (11-1) = 10, and 10 | 560 (560 / 10 = 56)
 *     p=17: (17-1) = 16, and 16 | 560 (560 / 16 = 35)
 *     Square-free? Yes. Composite? Yes. Thus 561 is Carmichael!
 * - Other small Carmichael numbers (OEIS A002997):
 *     1105, 1729, 2465, 2821, 6601, 8911...
 * 
 * Time Complexity:
 * - Korselt test: O(sqrt(n)) via prime factorization
 * - Fermat congruence test: O(n log n) checking all coprime bases
 * - Space Complexity: O(log n)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <numeric>

namespace carmichael {

/**
 * Computes (base^exp) % mod using binary modular exponentiation.
 */
int64_t powerMod(int64_t base, int64_t exp, int64_t mod) {
    int64_t res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) {
            res = static_cast<int64_t>((static_cast<__int128>(res) * base) % mod);
        }
        base = static_cast<int64_t>((static_cast<__int128>(base) * base) % mod);
        exp >>= 1;
    }
    return res;
}

/**
 * Computes greatest common divisor.
 */
int64_t gcd(int64_t a, int64_t b) {
    while (b) {
        int64_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

/**
 * Factors n into prime factors. Returns empty vector if n < 2.
 */
std::vector<int64_t> getPrimeFactors(int64_t n) {
    std::vector<int64_t> factors;
    for (int64_t d = 2; d * d <= n; ++d) {
        while (n % d == 0) {
            factors.push_back(d);
            n /= d;
        }
    }
    if (n > 1) {
        factors.push_back(n);
    }
    return factors;
}

/**
 * Checks if n is a Carmichael number using Korselt's Criterion.
 */
bool isCarmichaelKorselt(int64_t n) {
    if (n < 4) return false;

    auto factors = getPrimeFactors(n);

    // Condition 1: Must be composite (more than 1 prime factor)
    if (factors.size() <= 1) return false;

    // Condition 2: Must be square-free
    for (size_t i = 1; i < factors.size(); ++i) {
        if (factors[i] == factors[i - 1]) {
            return false;
        }
    }

    // Condition 3: For every prime factor p, (p - 1) | (n - 1)
    for (int64_t p : factors) {
        if ((n - 1) % (p - 1) != 0) {
            return false;
        }
    }

    return true;
}

/**
 * Checks if n satisfies Fermat's test for ALL coprime bases 1 < a < n.
 */
bool isCarmichaelByDefinition(int64_t n) {
    if (n < 4) return false;
    auto factors = getPrimeFactors(n);
    if (factors.size() <= 1) return false; // Must be composite

    for (int64_t a = 2; a < n; ++a) {
        if (gcd(a, n) == 1) {
            if (powerMod(a, n - 1, n) != 1) {
                return false;
            }
        }
    }
    return true;
}

} // namespace carmichael

int main() {
    std::cout << "========================================================\n";
    std::cout << " Carmichael Number Checker & Korselt's Criterion\n";
    std::cout << "========================================================\n\n";

    int64_t test_numbers[] = {
        561, 1105, 1729, 2465, 2821, 6601,
        560, 562, 1000, 1728, 997, 1009
    };

    std::cout << "Evaluating candidate numbers:\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << std::setw(10) << "Number"
              << std::setw(24) << "Prime Factorization"
              << std::setw(14) << "Korselt"
              << std::setw(16) << "Verdict\n";
    std::cout << "--------------------------------------------------------\n";

    for (int64_t n : test_numbers) {
        auto factors = carmichael::getPrimeFactors(n);
        std::string factor_str;
        for (size_t i = 0; i < factors.size(); ++i) {
            factor_str += std::to_string(factors[i]);
            if (i + 1 < factors.size()) factor_str += " * ";
        }

        bool korselt = carmichael::isCarmichaelKorselt(n);

        std::cout << std::setw(10) << n
                  << std::setw(24) << factor_str
                  << std::setw(14) << (korselt ? "PASS" : "FAIL")
                  << std::setw(16) << (korselt ? "CARMICHAEL" : "NOT CARMICHAEL")
                  << "\n";
    }

    std::cout << "--------------------------------------------------------\n\n";

    // Exhaustive search up to 10,000 using Korselt's Criterion
    std::cout << "Searching for all Carmichael numbers up to 10,000:\n";
    std::vector<int64_t> found;
    for (int64_t i = 2; i <= 10000; ++i) {
        if (carmichael::isCarmichaelKorselt(i)) {
            found.push_back(i);
        }
    }

    std::cout << "Found " << found.size() << " Carmichael numbers <= 10,000:\n  ";
    for (size_t i = 0; i < found.size(); ++i) {
        std::cout << found[i] << (i + 1 < found.size() ? ", " : "\n\n");
    }

    // Verify definition equivalence for the first Carmichael number 561
    std::cout << "Verifying definition a^(n-1) = 1 (mod n) for 561 on all coprime bases:\n";
    bool def_check = carmichael::isCarmichaelByDefinition(561);
    std::cout << "561 passes Fermat congruence for ALL coprime bases? "
              << (def_check ? "YES (VERIFIED)" : "NO") << "\n";

    return 0;
}
