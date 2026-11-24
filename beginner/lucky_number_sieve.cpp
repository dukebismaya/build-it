/*
 * ============================================================
 *                   Lucky Number Sieve
 * ============================================================
 *
 * Description:
 *   Lucky numbers (Gardiner & Metropolis, 1956) are produced by
 *   a sieve similar to Eratosthenes, but removing by POSITION:
 *     1. Start with the odd numbers 1, 3, 5, 7, 9, ...
 *     2. The second survivor is 3: delete every 3rd number.
 *     3. The next survivor is 7: delete every 7th number.
 *     4. Continue with each next survivor k: delete every k-th.
 *
 *   1, 3, 7, 9, 13, 15, 21, 25, 31, 33, 37, 43, 49, 51, ...
 *
 *   Lucky numbers share many statistical properties with
 *   primes (similar density ~ n / ln n, a "lucky Goldbach"
 *   conjecture, twin luckies, ...).
 *
 * Complexity:
 *   O(N * L) for the straightforward vector compaction, where L
 *   is the number of sieving rounds (small in practice).
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

namespace lucky {

    std::vector<int> sieve(int limit) {
        std::vector<int> v;
        for (int i = 1; i <= limit; i += 2) v.push_back(i);
        for (size_t idx = 1; idx < v.size(); ++idx) {
            size_t step = static_cast<size_t>(v[idx]);
            if (step > v.size()) break;
            std::vector<int> next;
            next.reserve(v.size());
            for (size_t i = 0; i < v.size(); ++i)
                if ((i + 1) % step != 0) next.push_back(v[i]);
            v.swap(next);
        }
        return v;
    }

} // namespace lucky

int main() {
    std::cout << "============================================================\n";
    std::cout << "                    Lucky Number Sieve\n";
    std::cout << "============================================================\n\n";

    auto small = lucky::sieve(300);
    std::cout << "Lucky numbers <= 300 (" << small.size() << "):\n";
    for (size_t i = 0; i < small.size(); ++i) {
        std::cout << std::setw(5) << small[i];
        if ((i + 1) % 12 == 0) std::cout << "\n";
    }

    const int expected[] = {1, 3, 7, 9, 13, 15, 21, 25, 31, 33, 37, 43, 49, 51, 63, 67, 69, 73, 75, 79};
    bool ok = true;
    for (int i = 0; i < 20; ++i) if (small[i] != expected[i]) ok = false;
    std::cout << "\n\nFirst 20 match OEIS A000959: " << (ok ? "PASSED" : "FAILED") << "\n";

    auto big = lucky::sieve(1000000);
    std::cout << "\nLucky numbers <= 10^6: " << big.size()
              << "  (n/ln n ~ " << static_cast<int>(1e6 / std::log(1e6)) << ")\n";

    int twins = 0;
    for (size_t i = 1; i < big.size(); ++i) if (big[i] - big[i - 1] == 2) ++twins;
    std::cout << "Twin lucky pairs <= 10^6: " << twins << "\n";
    return 0;
}
