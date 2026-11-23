/*
 * ============================================================
 *          Hamming Weight (Popcount) Analysis
 * ============================================================
 *
 * Description:
 *   The Hamming weight of an integer is the number of 1-bits in
 *   its binary representation. This program compares several
 *   classic ways to compute it:
 *     1. Naive bit-by-bit loop            O(bits)
 *     2. Kernighan's trick  x &= x - 1    O(popcount)
 *     3. 8-bit lookup table               O(bytes)
 *     4. SWAR parallel bit counting       O(1)
 *   and verifies them against each other.
 *
 * SWAR formula (64-bit):
 *   x = x - ((x >> 1) & 0x5555...)
 *   x = (x & 0x3333...) + ((x >> 2) & 0x3333...)
 *   x = (x + (x >> 4)) & 0x0F0F...
 *   count = (x * 0x0101...) >> 56
 * ============================================================
 */

#include <iostream>
#include <cstdint>
#include <array>
#include <random>
#include <iomanip>

namespace popcount {

    using u64 = uint64_t;

    int naive(u64 x) { int c = 0; while (x) { c += x & 1; x >>= 1; } return c; }

    int kernighan(u64 x) { int c = 0; while (x) { x &= x - 1; ++c; } return c; }

    const std::array<int, 256>& table() {
        static std::array<int, 256> t = [] {
            std::array<int, 256> a{};
            for (int i = 1; i < 256; ++i) a[i] = a[i >> 1] + (i & 1);
            return a;
        }();
        return t;
    }

    int lookup(u64 x) {
        int c = 0;
        for (int i = 0; i < 8; ++i) { c += table()[x & 0xFF]; x >>= 8; }
        return c;
    }

    int swar(u64 x) {
        x = x - ((x >> 1) & 0x5555555555555555ULL);
        x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
        x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
        return static_cast<int>((x * 0x0101010101010101ULL) >> 56);
    }

} // namespace popcount

int main() {
    using namespace popcount;
    std::cout << "============================================================\n";
    std::cout << "             Hamming Weight (Popcount) Analysis\n";
    std::cout << "============================================================\n\n";

    u64 samples[] = {0, 1, 7, 255, 1023, 0xDEADBEEFULL, 0xFFFFFFFFFFFFFFFFULL, 0x8000000000000001ULL};
    std::cout << std::left << std::setw(22) << "Value" << "Naive Kern  LUT  SWAR\n";
    for (u64 v : samples)
        std::cout << std::setw(22) << v << std::setw(6) << naive(v) << std::setw(6) << kernighan(v)
                  << std::setw(5) << lookup(v) << swar(v) << "\n";

    std::mt19937_64 rng(1);
    bool ok = true;
    for (int i = 0; i < 100000; ++i) {
        u64 x = rng();
        int r = naive(x);
        if (kernighan(x) != r || lookup(x) != r || swar(x) != r) { ok = false; break; }
    }
    std::cout << "\nRandom cross-check (100000 values): " << (ok ? "PASSED" : "FAILED") << "\n";

    // Distribution of popcount over 0..2^16-1 follows binomial C(16, k)
    std::array<int, 17> dist{};
    for (u64 x = 0; x < (1u << 16); ++x) ++dist[swar(x)];
    std::cout << "\nPopcount distribution over 16-bit integers (= C(16,k)):\n";
    for (int k = 0; k <= 16; ++k) std::cout << "  k=" << std::setw(2) << k << ": " << dist[k] << "\n";
    return 0;
}
