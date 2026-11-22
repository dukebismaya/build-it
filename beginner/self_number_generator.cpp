/*
 * ============================================================
 *          Self Number (Colombian Number) Generator
 * ============================================================
 *
 * Description:
 *   Define d(n) = n + (sum of digits of n). A self number is
 *   an integer that cannot be written as d(m) for any m.
 *   Introduced by D. R. Kaprekar (1949).
 *
 *   1, 3, 5, 7, 9, 20, 31, 42, 53, 64, 75, 86, 97, 108, ...
 *
 * Algorithm (sieve):
 *   Mark generated[d(m)] = true for every m <= LIMIT.
 *   Every unmarked n in [1, LIMIT] is a self number.
 *   (d(m) > m, so all generators of n <= LIMIT are <= LIMIT.)
 *
 * Complexity:
 *   O(L * log10 L) time, O(L) space
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>

namespace self_numbers {

    int digit_sum(int n) {
        int s = 0;
        while (n) { s += n % 10; n /= 10; }
        return s;
    }

    std::vector<int> sieve(int limit) {
        std::vector<bool> generated(limit + 1, false);
        for (int m = 1; m <= limit; ++m) {
            long long d = static_cast<long long>(m) + digit_sum(m);
            if (d <= limit) generated[d] = true;
        }
        std::vector<int> result;
        for (int n = 1; n <= limit; ++n)
            if (!generated[n]) result.push_back(n);
        return result;
    }

    // Direct check: any generator m of n lies in [n - 9*digits, n)
    bool is_self(int n) {
        for (int m = std::max(1, n - 9 * 10); m < n; ++m)
            if (m + digit_sum(m) == n) return false;
        return true;
    }

} // namespace self_numbers

int main() {
    std::cout << "============================================================\n";
    std::cout << "        Self Number (Colombian Number) Generator\n";
    std::cout << "============================================================\n\n";

    auto list = self_numbers::sieve(1000);
    std::cout << "Self numbers <= 1000 (" << list.size() << " total):\n";
    for (size_t i = 0; i < list.size(); ++i) {
        std::cout << std::setw(5) << list[i];
        if ((i + 1) % 12 == 0) std::cout << "\n";
    }

    bool ok = true;
    for (int n = 1; n <= 1000; ++n) {
        bool in_list = std::binary_search(list.begin(), list.end(), n);
        if (in_list != self_numbers::is_self(n)) ok = false;
    }
    std::cout << "\n\nSieve vs direct check: " << (ok ? "PASSED" : "FAILED") << "\n";

    auto big = self_numbers::sieve(1000000);
    std::cout << "Self numbers <= 10^6: " << big.size()
              << " (density ~" << std::fixed << std::setprecision(4)
              << big.size() / 1e6 << ", known ~0.0977)\n";
    return 0;
}
