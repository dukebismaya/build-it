/*
 * ============================================================
 *              Sociable Number Chain Finder
 * ============================================================
 *
 * Description:
 *   Sociable numbers form cycles under the aliquot function
 *   s(n) = sigma(n) - n (sum of proper divisors).
 *     - Cycle length 1: perfect numbers   (6, 28, 496, ...)
 *     - Cycle length 2: amicable pairs    (220 <-> 284)
 *     - Cycle length >2: sociable chains  (12496 -> ... -> 12496)
 *
 * Algorithm:
 *   1. Precompute s(n) for all n <= LIMIT with a divisor sieve.
 *   2. For each start n, iterate s() up to MAX_LEN steps; if we
 *      return to n without exceeding LIMIT, it is a cycle.
 *   3. Report each cycle once (only when n is its smallest member).
 *
 * Complexity:
 *   Sieve: O(L log L), search: O(L * MAX_LEN), space O(L)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <algorithm>

namespace sociable {

    std::vector<long long> aliquot_sieve(int limit) {
        std::vector<long long> s(limit + 1, 0);
        for (int d = 1; d <= limit / 2; ++d)
            for (int m = 2 * d; m <= limit; m += d)
                s[m] += d;
        return s;
    }

    std::vector<std::vector<int>> find_cycles(int limit, int max_len) {
        auto s = aliquot_sieve(limit);
        std::vector<std::vector<int>> cycles;
        for (int n = 2; n <= limit; ++n) {
            std::vector<int> chain{n};
            long long cur = s[n];
            bool ok = false;
            for (int step = 0; step < max_len; ++step) {
                if (cur < n || cur > limit) break;  // smallest-member rule / bound
                if (cur == n) { ok = true; break; }
                chain.push_back(static_cast<int>(cur));
                cur = s[cur];
            }
            if (!ok) continue;
            // reject chains that loop internally without hitting n
            std::vector<int> sorted = chain;
            std::sort(sorted.begin(), sorted.end());
            if (std::adjacent_find(sorted.begin(), sorted.end()) != sorted.end()) continue;
            cycles.push_back(chain);
        }
        return cycles;
    }

} // namespace sociable

int main() {
    const int LIMIT = 1500000;
    const int MAX_LEN = 30;

    std::cout << "============================================================\n";
    std::cout << "          Sociable Number Chain Finder (n <= " << LIMIT << ")\n";
    std::cout << "============================================================\n\n";

    auto cycles = sociable::find_cycles(LIMIT, MAX_LEN);
    int perfect = 0, amicable = 0, social = 0;

    for (const auto& c : cycles) {
        const char* kind = c.size() == 1 ? "Perfect " : c.size() == 2 ? "Amicable" : "Sociable";
        if (c.size() == 1) ++perfect; else if (c.size() == 2) ++amicable; else ++social;
        std::cout << kind << " (len " << c.size() << "): ";
        for (size_t i = 0; i < c.size(); ++i) std::cout << c[i] << " -> ";
        std::cout << c[0] << "\n";
    }

    std::cout << "\nPerfect: " << perfect << ", Amicable pairs: " << amicable
              << ", Sociable chains (len>2): " << social << "\n";
    std::cout << "Known check: 12496 chain has length 5, 14316 chain has length 28.\n";
    return 0;
}
