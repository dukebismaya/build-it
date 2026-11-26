/*
 * ============================================================
 *                  Leyland Number Generator
 * ============================================================
 *
 * Description:
 *   Named after mathematician Paul Leyland, a Leyland number is
 *   an integer of the form:
 *     L(x, y) = x^y + y^x    with integers x >= y > 1
 *   First few Leyland numbers (OEIS A076980):
 *     8, 17, 32, 54, 57, 100, 145, 177, 320, 368, 512, 593, 945, 1124
 *
 *   A Leyland number of the second kind has the form:
 *     L_2(x, y) = x^y - y^x  with integers x > y > 1
 *
 *   Leyland primes (primes of the form x^y + y^x) include:
 *     17, 593, 32993, 2097593, 8589935681, ...
 *
 * Complexity:
 *   Enumerating all Leyland numbers <= LIMIT takes O((log LIMIT)^2)
 *   pairs (x, y) and O(M log M) sorting.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <iomanip>

namespace leyland {

    using u64 = uint64_t;
    using u128 = __uint128_t;

    struct Term {
        u64 value;
        int x, y;
        bool operator<(const Term& o) const { return value < o.value; }
    };

    bool safe_pow(u64 base, int exp, u64 limit, u64& out) {
        u128 r = 1;
        for (int i = 0; i < exp; ++i) {
            r *= base;
            if (r > limit) return false;
        }
        out = static_cast<u64>(r);
        return true;
    }

    std::vector<Term> generate_first_kind(u64 limit) {
        std::vector<Term> list;
        for (int y = 2; ; ++y) {
            u64 yy = 0;
            if (!safe_pow(y, y, limit, yy) || 2 * yy > limit) break;
            for (int x = y; ; ++x) {
                u64 xy = 0, yx = 0;
                if (!safe_pow(x, y, limit, xy)) break;
                if (!safe_pow(y, x, limit - xy, yx)) break;
                list.push_back({xy + yx, x, y});
            }
        }
        std::sort(list.begin(), list.end());
        return list;
    }

    bool is_prime(u64 n) {
        if (n < 2) return false;
        if (n % 2 == 0) return n == 2;
        if (n % 3 == 0) return n == 3;
        for (u64 d = 5; d * d <= n; d += 6)
            if (n % d == 0 || n % (d + 2) == 0) return false;
        return true;
    }

} // namespace leyland

int main() {
    using namespace leyland;
    std::cout << "============================================================\n";
    std::cout << "                  Leyland Number Generator\n";
    std::cout << "============================================================\n\n";

    const u64 LIMIT = 1000000000000ULL; // 10^12
    auto terms = generate_first_kind(LIMIT);

    std::vector<u64> unique_vals;
    for (const auto& t : terms)
        if (unique_vals.empty() || unique_vals.back() != t.value)
            unique_vals.push_back(t.value);

    std::cout << "First 20 Leyland numbers (x^y + y^x <= 10^12):\n";
    for (int i = 0; i < 20; ++i) {
        const auto& t = terms[i];
        std::cout << "  " << std::setw(8) << t.value
                  << " = " << t.x << "^" << t.y << " + " << t.y << "^" << t.x
                  << (is_prime(t.value) ? "  [PRIME]" : "") << "\n";
    }

    std::vector<u64> primes;
    for (u64 v : unique_vals) if (is_prime(v)) primes.push_back(v);
    std::cout << "\nLeyland primes <= 10^12 (" << primes.size() << " found):\n  ";
    for (u64 p : primes) std::cout << p << " ";
    std::cout << "\n";

    const u64 expected_first14[] = {8, 17, 32, 54, 57, 100, 145, 177, 320, 368, 512, 593, 945, 1124};
    const u64 expected_primes[]  = {17, 593, 32993, 2097593, 8589935681ULL, 59604644783353249ULL};
    bool ok = true;
    for (int i = 0; i < 14; ++i) if (unique_vals[i] != expected_first14[i]) ok = false;
    for (int i = 0; i < 5; ++i)  if (primes[i] != expected_primes[i]) ok = false;

    std::cout << "\nVerification against OEIS A076980 & A094133: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
