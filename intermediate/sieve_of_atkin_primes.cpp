/*
 * ============================================================
 *                   Sieve of Atkin
 * ============================================================
 *
 * Description:
 *   The Sieve of Atkin finds all primes up to N using binary
 *   quadratic forms instead of crossing out multiples. For a
 *   square-free n > 3, n is prime iff it has an ODD number of
 *   solutions to the form selected by n mod 12:
 *
 *     n mod 12 in {1, 5}  : 4x^2 + y^2 = n
 *     n mod 12 == 7       : 3x^2 + y^2 = n
 *     n mod 12 == 11      : 3x^2 - y^2 = n   (x > y)
 *
 * Algorithm:
 *   1. Toggle is_prime[n] for every (x, y) solution above.
 *   2. Eliminate multiples of squares of primes (non-square-free).
 *   3. Add 2 and 3.
 *
 * Complexity:
 *   O(N) operations (O(N / log log N) with wheel optimizations),
 *   O(N) space.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <chrono>

namespace atkin {

    std::vector<int> sieve(int limit) {
        std::vector<bool> is(limit + 1, false);
        for (long long x = 1; x * x <= limit; ++x) {
            for (long long y = 1; y * y <= limit; ++y) {
                long long n = 4 * x * x + y * y;
                if (n <= limit && (n % 12 == 1 || n % 12 == 5)) is[n] = !is[n];
                n = 3 * x * x + y * y;
                if (n <= limit && n % 12 == 7) is[n] = !is[n];
                n = 3 * x * x - y * y;
                if (x > y && n <= limit && n % 12 == 11) is[n] = !is[n];
            }
        }
        for (long long r = 5; r * r <= limit; ++r)
            if (is[r])
                for (long long k = r * r; k <= limit; k += r * r) is[k] = false;

        std::vector<int> primes;
        if (limit >= 2) primes.push_back(2);
        if (limit >= 3) primes.push_back(3);
        for (int n = 5; n <= limit; ++n) if (is[n]) primes.push_back(n);
        return primes;
    }

    std::vector<int> eratosthenes(int limit) {
        std::vector<bool> comp(limit + 1, false);
        std::vector<int> primes;
        for (long long i = 2; i <= limit; ++i) {
            if (comp[i]) continue;
            primes.push_back(static_cast<int>(i));
            for (long long j = i * i; j <= limit; j += i) comp[j] = true;
        }
        return primes;
    }

} // namespace atkin

int main() {
    std::cout << "============================================================\n";
    std::cout << "                     Sieve of Atkin\n";
    std::cout << "============================================================\n\n";

    auto small = atkin::sieve(100);
    std::cout << "Primes <= 100 (" << small.size() << "): ";
    for (int p : small) std::cout << p << " ";
    std::cout << "\n\n";

    for (int limit : {1000, 100000, 10000000}) {
        auto t0 = std::chrono::steady_clock::now();
        auto a = atkin::sieve(limit);
        auto t1 = std::chrono::steady_clock::now();
        auto e = atkin::eratosthenes(limit);
        auto t2 = std::chrono::steady_clock::now();
        std::cout << "N = " << limit << ": pi(N) = " << a.size()
                  << " | Atkin " << std::chrono::duration<double, std::milli>(t1 - t0).count() << " ms"
                  << " | Eratosthenes " << std::chrono::duration<double, std::milli>(t2 - t1).count() << " ms"
                  << " | match: " << (a == e ? "YES" : "NO") << "\n";
    }
    std::cout << "\nKnown values: pi(1000)=168, pi(10^5)=9592, pi(10^7)=664579\n";
    return 0;
}
