/**
 * ============================================================================
 * Practical (Panarithmic) Number Checker & Stewart-Sierpinski Verification
 * ============================================================================
 * 
 * Description:
 * A practical number (also called a panarithmic number) is a positive integer n
 * such that every integer k in [1, n] can be expressed as the sum of distinct
 * divisors of n.
 * 
 * Examples:
 * - 6 has divisors {1, 2, 3, 6}:
 *     1 = 1
 *     2 = 2
 *     3 = 3
 *     4 = 1 + 3
 *     5 = 2 + 3
 *     6 = 6
 *   Every integer 1..6 is representable, so 6 is practical.
 * - 10 has divisors {1, 2, 5, 10}:
 *     Possible sums from {1, 2, 5}: 1, 2, 3 (1+2), 5, 6 (1+5), 7 (2+5), 8 (1+2+5)
 *     Notice that 4 CANNOT be formed! Thus, 10 is NOT practical.
 * 
 * Stewart-Sierpinski Criterion (1954-1955, noted by Ramanujan in 1915):
 * Let n = p_1^{a_1} * p_2^{a_2} * ... * p_k^{a_k} be the prime factorization
 * with 2 <= p_1 < p_2 < ... < p_k.
 * Then n is practical if and only if:
 *   1) p_1 = 2 (for n > 1; 1 is considered practical by definition).
 *   2) For each i from 2 to k:
 *        p_i <= 1 + sigma(p_1^{a_1} * ... * p_{i-1}^{a_{i-1}})
 *      where sigma(m) is the sum of all positive divisors of m.
 * 
 * First Practical Numbers (OEIS A005153):
 *   1, 2, 4, 6, 8, 12, 16, 18, 20, 24, 28, 30, 32, 36, 40, 42, 48, ...
 * 
 * Time Complexity:
 * - Stewart-Sierpinski test: O(sqrt(n)) via prime factorization
 * - Subset-sum definition check: O(n * |divisors|)
 * - Space Complexity: O(number of prime factors)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <algorithm>
#include <numeric>

namespace practical {

struct PrimeFactor {
    int64_t prime;
    int exponent;
};

/**
 * Prime factorizes an integer n into prime-power pairs.
 */
std::vector<PrimeFactor> factorize(int64_t n) {
    std::vector<PrimeFactor> factors;
    for (int64_t d = 2; d * d <= n; ++d) {
        if (n % d == 0) {
            int count = 0;
            while (n % d == 0) {
                count++;
                n /= d;
            }
            factors.push_back({d, count});
        }
    }
    if (n > 1) {
        factors.push_back({n, 1});
    }
    return factors;
}

/**
 * Computes sigma(p^a) = (p^(a+1) - 1) / (p - 1).
 */
int64_t sigmaPrimePower(int64_t p, int a) {
    int64_t sum = 1;
    int64_t cur = 1;
    for (int i = 0; i < a; ++i) {
        cur *= p;
        sum += cur;
    }
    return sum;
}

/**
 * Tests whether n is practical using the Stewart-Sierpinski Criterion in O(sqrt(n)).
 */
bool isPracticalStewart(int64_t n) {
    if (n <= 0) return false;
    if (n == 1) return true;

    auto factors = factorize(n);

    // Condition 1: Must be even (smallest prime factor must be 2)
    if (factors.empty() || factors[0].prime != 2) {
        return false;
    }

    // sigma_accum tracks sigma(p_1^{a_1} * ... * p_{i-1}^{a_{i-1}})
    int64_t sigma_accum = sigmaPrimePower(factors[0].prime, factors[0].exponent);

    // Condition 2: p_i <= 1 + sigma_accum for all subsequent primes
    for (size_t i = 1; i < factors.size(); ++i) {
        if (factors[i].prime > 1 + sigma_accum) {
            return false;
        }
        sigma_accum *= sigmaPrimePower(factors[i].prime, factors[i].exponent);
    }

    return true;
}

/**
 * Finds all divisors of n.
 */
std::vector<int64_t> getDivisors(int64_t n) {
    std::vector<int64_t> divs;
    for (int64_t d = 1; d * d <= n; ++d) {
        if (n % d == 0) {
            divs.push_back(d);
            if (d * d != n) {
                divs.push_back(n / d);
            }
        }
    }
    std::sort(divs.begin(), divs.end());
    return divs;
}

/**
 * Tests whether n is practical using the brute-force definition:
 * Every integer k in [1, n] must be representable as sum of distinct divisors.
 */
bool isPracticalDefinition(int64_t n) {
    if (n <= 0) return false;
    if (n == 1) return true;

    auto divs = getDivisors(n);

    // DP subset sum array up to n
    std::vector<bool> reachable(n + 1, false);
    reachable[0] = true;

    for (int64_t d : divs) {
        for (int64_t val = n; val >= d; --val) {
            if (reachable[val - d]) {
                reachable[val] = true;
            }
        }
    }

    for (int64_t k = 1; k <= n; ++k) {
        if (!reachable[k]) return false;
    }

    return true;
}

} // namespace practical

int main() {
    std::cout << "========================================================\n";
    std::cout << " Practical Number Checker (Stewart-Sierpinski Theorem)\n";
    std::cout << "========================================================\n\n";

    std::cout << "Finding all practical numbers up to 100:\n";
    std::vector<int64_t> practicals;
    for (int64_t i = 1; i <= 100; ++i) {
        bool by_stewart = practical::isPracticalStewart(i);
        bool by_def = practical::isPracticalDefinition(i);

        if (by_stewart != by_def) {
            std::cerr << "Mismatch found at " << i << "!\n";
            return 1;
        }

        if (by_stewart) {
            practicals.push_back(i);
        }
    }

    std::cout << "Total found <= 100: " << practicals.size() << "\n  ";
    for (size_t i = 0; i < practicals.size(); ++i) {
        std::cout << practicals[i] << (i + 1 < practicals.size() ? ", " : "\n\n");
    }

    std::cout << "Detailed divisor sum breakdown for select numbers:\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << std::setw(8) << "Number"
              << std::setw(15) << "Stewart Test"
              << std::setw(15) << "Def. Test"
              << std::setw(16) << "Verdict\n";
    std::cout << "--------------------------------------------------------\n";

    int64_t test_cases[] = {1, 2, 6, 10, 12, 18, 20, 22, 24, 30, 34, 42, 60, 84, 100};
    for (int64_t tc : test_cases) {
        bool s_val = practical::isPracticalStewart(tc);
        bool d_val = practical::isPracticalDefinition(tc);

        std::cout << std::setw(8) << tc
                  << std::setw(15) << (s_val ? "PASS" : "FAIL")
                  << std::setw(15) << (d_val ? "PASS" : "FAIL")
                  << std::setw(16) << (s_val ? "PRACTICAL" : "NOT PRACTICAL")
                  << "\n";
    }

    std::cout << "--------------------------------------------------------\n";
    std::cout << "All Stewart-Sierpinski tests matched definition tests 100%.\n";
    return 0;
}
