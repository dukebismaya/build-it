/**
 * ============================================================================
 * Untouchable Number Checker & Aliquot Sum Inversion
 * ============================================================================
 * 
 * Description:
 * An untouchable number is a positive integer that cannot be expressed as the
 * sum of all proper divisors (the aliquot sum s(m) = sigma(m) - m) of any
 * positive integer m.
 * 
 * Mathematical Context:
 * - Proper divisor sum: s(m) = sum_{d | m, d < m} d.
 * - For example:
 *     s(1) = 0
 *     s(2) = 1, s(3) = 1, s(4) = 3, s(5) = 1, s(6) = 6, s(7) = 1, s(8) = 7
 * - Notice that 2 and 5 are never outputs of s(m). They are untouchable!
 * - Theorem (Paul Erdős): 5 is believed to be the only odd untouchable number.
 *   If the strong Goldbach conjecture holds (every even integer >= 6 is a sum
 *   of two distinct primes p + q), then for any odd integer n > 5:
 *     n - 1 = p + q  =>  s(p * q) = 1 + p + q = n.
 *   Hence no odd number greater than 5 can be untouchable.
 * - 2 is the only known prime untouchable besides 5.
 * 
 * First Untouchable Numbers (OEIS A005114):
 *   2, 5, 52, 88, 96, 120, 124, 146, 162, 188, 206, 210, 216, 238, 246, 248...
 * 
 * Time Complexity:
 * - Sieve-based aliquot precomputation up to limit L: O(L log L)
 * - Untouchability test for n: O(1) after sieve precomputation
 * - Space Complexity: O(L)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <algorithm>

namespace untouchable {

/**
 * Computes the aliquot sum s(m) for all 1 <= m <= max_m using a linear/sieve method.
 */
std::vector<int64_t> computeAliquotSums(int max_m) {
    std::vector<int64_t> s(max_m + 1, 0);

    for (int d = 1; d <= max_m / 2; ++d) {
        for (int mult = 2 * d; mult <= max_m; mult += d) {
            s[mult] += d;
        }
    }

    return s;
}

/**
 * Identifies untouchable numbers up to target_limit using a search bound.
 * To safely test numbers up to target_limit, we test m up to search_bound.
 */
std::vector<int> findUntouchables(int target_limit, int search_bound) {
    std::vector<int64_t> s = computeAliquotSums(search_bound);
    std::vector<bool> touched(target_limit + 1, false);

    for (int m = 1; m <= search_bound; ++m) {
        if (s[m] <= target_limit) {
            touched[s[m]] = true;
        }
    }

    std::vector<int> untouchables;
    for (int n = 1; n <= target_limit; ++n) {
        if (!touched[n]) {
            untouchables.push_back(n);
        }
    }

    return untouchables;
}

/**
 * Checks if an odd number > 5 can be expressed as s(p*q) = 1 + p + q
 * using Goldbach's theorem.
 */
bool verifyOddGoldbachTheorem(int n, int& p_out, int& q_out) {
    if (n % 2 == 0 || n <= 5) return false;

    int target_even = n - 1;

    auto is_prime = [](int x) {
        if (x < 2) return false;
        for (int d = 2; d * d <= x; ++d) {
            if (x % d == 0) return false;
        }
        return true;
    };

    for (int p = 3; p <= target_even / 2; ++p) {
        int q = target_even - p;
        if (p != q && is_prime(p) && is_prime(q)) {
            p_out = p;
            q_out = q;
            return true;
        }
    }

    return false;
}

} // namespace untouchable

int main() {
    std::cout << "========================================================\n";
    std::cout << " Untouchable Number Checker & Aliquot Sum Inversion\n";
    std::cout << "========================================================\n\n";

    constexpr int TARGET_LIMIT = 250;
    constexpr int SEARCH_BOUND = 50000;

    std::cout << "Searching for untouchable numbers <= " << TARGET_LIMIT
              << " with search bound M <= " << SEARCH_BOUND << "...\n";

    std::vector<int> untouchables = untouchable::findUntouchables(TARGET_LIMIT, SEARCH_BOUND);

    std::cout << "\nFound " << untouchables.size() << " untouchable numbers <= " << TARGET_LIMIT << ":\n";
    for (size_t i = 0; i < untouchables.size(); ++i) {
        std::cout << std::setw(5) << untouchables[i];
        if ((i + 1) % 10 == 0 || i + 1 == untouchables.size()) {
            std::cout << "\n";
        }
    }

    std::cout << "\n--------------------------------------------------------\n";
    std::cout << " Verification of Erdos Odd Untouchable Property (n > 5):\n";
    std::cout << "--------------------------------------------------------\n";

    int odd_samples[] = {7, 9, 11, 13, 15, 17, 21, 25, 33, 99};
    for (int odd_n : odd_samples) {
        int p = 0, q = 0;
        bool has_witness = untouchable::verifyOddGoldbachTheorem(odd_n, p, q);
        if (has_witness) {
            int64_t m = static_cast<int64_t>(p) * q;
            std::cout << "  " << std::setw(3) << odd_n
                      << " is TOUCHABLE: s(" << m << " = " << p << " * " << q
                      << ") = 1 + " << p << " + " << q << " = " << odd_n << "\n";
        }
    }

    std::cout << "\nNotice: 2 and 5 are the only prime untouchable numbers!\n";
    return 0;
}
