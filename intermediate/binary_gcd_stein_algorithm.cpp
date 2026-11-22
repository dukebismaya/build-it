/*
 * ============================================================
 *             Binary GCD (Stein's Algorithm)
 * ============================================================
 *
 * Description:
 *   Stein's algorithm computes gcd(a, b) using only shifts,
 *   subtraction and comparison — no division. It relies on:
 *     gcd(0, b)        = b
 *     gcd(2a, 2b)      = 2 * gcd(a, b)
 *     gcd(2a, b odd)   = gcd(a, b)
 *     gcd(a, b) both odd, a >= b  ->  gcd(a - b, b)
 *
 *   Using count-trailing-zeros, the common power of two is
 *   factored out once, and odd parts are reduced iteratively.
 *
 * Complexity:
 *   O(log a + log b) iterations, each O(1) bit operations.
 * ============================================================
 */

#include <iostream>
#include <cstdint>
#include <numeric>
#include <random>
#include <chrono>
#include <utility>

namespace stein {

    using u64 = uint64_t;

    int ctz(u64 x) {
        int n = 0;
        while (!(x & 1)) { x >>= 1; ++n; }
        return n;
    }

    u64 binary_gcd(u64 a, u64 b) {
        if (a == 0) return b;
        if (b == 0) return a;
        int shift = ctz(a | b);   // common factors of 2
        a >>= ctz(a);
        do {
            b >>= ctz(b);
            if (a > b) std::swap(a, b);
            b -= a;
        } while (b != 0);
        return a << shift;
    }

    // Recursive textbook form for comparison
    u64 binary_gcd_recursive(u64 a, u64 b) {
        if (a == b) return a;
        if (a == 0) return b;
        if (b == 0) return a;
        if (~a & 1) {
            if (b & 1) return binary_gcd_recursive(a >> 1, b);
            return binary_gcd_recursive(a >> 1, b >> 1) << 1;
        }
        if (~b & 1) return binary_gcd_recursive(a, b >> 1);
        if (a > b) return binary_gcd_recursive((a - b) >> 1, b);
        return binary_gcd_recursive((b - a) >> 1, a);
    }

} // namespace stein

int main() {
    using namespace stein;
    std::cout << "============================================================\n";
    std::cout << "              Binary GCD (Stein's Algorithm)\n";
    std::cout << "============================================================\n\n";

    std::pair<u64, u64> samples[] = {
        {48, 18}, {270, 192}, {17, 31}, {0, 9}, {1ULL << 40, 1ULL << 25},
        {1071, 462}, {123456789012ULL, 9876543210ULL}
    };
    for (auto& p : samples) {
        std::cout << "gcd(" << p.first << ", " << p.second << ") = "
                  << binary_gcd(p.first, p.second)
                  << "  [std::gcd = " << std::gcd(p.first, p.second) << "]\n";
    }

    std::mt19937_64 rng(2026);
    const int TRIALS = 200000;
    bool ok = true;
    for (int i = 0; i < TRIALS; ++i) {
        u64 a = rng() >> (rng() % 40), b = rng() >> (rng() % 40);
        u64 g = binary_gcd(a, b);
        if (g != std::gcd(a, b) || g != binary_gcd_recursive(a, b)) { ok = false; break; }
    }
    std::cout << "\nRandom verification (" << TRIALS << " pairs): "
              << (ok ? "PASSED" : "FAILED") << "\n";

    auto time_it = [&](auto fn) {
        std::mt19937_64 r(7);
        u64 acc = 0;
        auto t0 = std::chrono::steady_clock::now();
        for (int i = 0; i < 1000000; ++i) acc += fn(r(), r());
        auto t1 = std::chrono::steady_clock::now();
        std::cout << std::chrono::duration<double, std::milli>(t1 - t0).count()
                  << " ms (checksum " << acc % 1000 << ")\n";
    };
    std::cout << "\n1M gcds, binary  : "; time_it([](u64 a, u64 b) { return binary_gcd(a, b); });
    std::cout << "1M gcds, Euclid  : "; time_it([](u64 a, u64 b) { return std::gcd(a, b); });
    return 0;
}
