/*
 * ============================================================
 *            Digit Factorial Chain Analyzer
 * ============================================================
 *
 * Description:
 *   Let f(n) = sum of the factorials of the digits of n, e.g.
 *     f(145) = 1! + 4! + 5! = 145      (a factorion)
 *     f(69)  = 6! + 9! = 363600
 *
 *   Repeatedly applying f always ends in a loop. The only
 *   loops are:
 *     length 1: 1, 2, 145, 40585        (factorions)
 *     length 2: 871 <-> 45361, 872 <-> 45362
 *     length 3: 169 -> 363601 -> 1454 -> 169
 *
 *   The "chain length" of n is the number of distinct terms
 *   before a repeat. Project Euler #74 asks how many starting
 *   numbers below one million produce exactly 60 non-repeating
 *   terms.
 *
 * Algorithm:
 *   Memoize chain lengths. Walk the chain storing the path;
 *   when we hit a known value or a repeat, back-fill lengths.
 *
 * Complexity: O(N) amortized with memoization.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <iomanip>

namespace digit_factorial {

    const int FACT[10] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880};

    int f(int n) {
        int s = 0;
        do { s += FACT[n % 10]; n /= 10; } while (n);
        return s;
    }

    class ChainCache {
        std::vector<int> memo;   // 0 = unknown
    public:
        explicit ChainCache(int max_value) : memo(max_value + 1, 0) {}

        int length(int start) {
            if (memo[start]) return memo[start];
            std::vector<int> path;
            std::unordered_map<int, int> pos;
            int x = start;
            while (!memo[x] && !pos.count(x)) {
                pos[x] = static_cast<int>(path.size());
                path.push_back(x);
                x = f(x);
            }
            int tail;
            int stop;
            if (memo[x]) { tail = memo[x]; stop = static_cast<int>(path.size()); }
            else {
                int cycle_start = pos[x];
                int cycle_len = static_cast<int>(path.size()) - cycle_start;
                for (int i = cycle_start; i < static_cast<int>(path.size()); ++i) memo[path[i]] = cycle_len;
                tail = cycle_len; stop = cycle_start;
            }
            for (int i = stop - 1; i >= 0; --i) memo[path[i]] = ++tail;
            return memo[start];
        }
    };

    int brute_length(int n) {
        std::vector<int> seen;
        while (true) {
            for (int s : seen) if (s == n) return static_cast<int>(seen.size());
            seen.push_back(n);
            n = f(n);
        }
    }

} // namespace digit_factorial

int main() {
    using namespace digit_factorial;
    std::cout << "============================================================\n";
    std::cout << "              Digit Factorial Chain Analyzer\n";
    std::cout << "============================================================\n\n";

    for (int start : {145, 169, 871, 69, 78, 540}) {
        std::cout << start;
        std::vector<int> seen{start};
        int x = start;
        while (true) {
            x = f(x);
            bool rep = false;
            for (int s : seen) if (s == x) rep = true;
            std::cout << " -> " << x;
            if (rep) break;
            seen.push_back(x);
        }
        std::cout << "   (" << seen.size() << " distinct terms)\n";
    }

    ChainCache cache(2600000);   // f(n) < 7 * 9! = 2540160 for n < 10^7
    int count60 = 0, longest = 0;
    bool ok = true;
    for (int n = 1; n < 1000000; ++n) {
        int len = cache.length(n);
        if (len == 60) ++count60;
        if (len > longest) longest = len;
        if (n < 3000 && len != brute_length(n)) ok = false;
    }
    std::cout << "\nMemoized vs brute force (n < 3000): " << (ok ? "PASSED" : "FAILED") << "\n";
    std::cout << "Starting numbers < 10^6 with exactly 60 terms: " << count60 << " (Project Euler #74: 402)\n";
    std::cout << "Longest chain below 10^6: " << longest << " terms\n";
    return 0;
}
