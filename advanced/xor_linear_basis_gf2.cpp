/*
 * ============================================================
 *        XOR Linear Basis (Gaussian Elimination over GF(2))
 * ============================================================
 *
 * Description:
 *   Treat each 64-bit integer as a vector over GF(2). A linear
 *   basis keeps at most 64 vectors, at most one per leading
 *   bit, spanning exactly the set of all XORs of subsets of the
 *   inserted numbers.
 *
 *   insert(x): for bit b from high to low, if x has bit b:
 *                if basis[b] empty -> basis[b] = x, done
 *                else x ^= basis[b]
 *              if x becomes 0 it was already representable.
 *
 * Queries:
 *   - max_xor():        greedy from the top bit          O(64)
 *   - min_xor():        smallest basis vector (or 0 if some
 *                       insertion was dependent)          O(64)
 *   - contains(x):      reduce x; representable iff 0     O(64)
 *   - kth_smallest(k):  reduce basis to reduced row-echelon
 *                       form, then the binary digits of k pick
 *                       which basis vectors to XOR        O(64)
 *   - count:            2^rank distinct subset XORs
 *
 * Complexity: O(64) per insert/query, O(64) space.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <set>
#include <random>
#include <algorithm>

namespace xor_basis {

    using u64 = uint64_t;

    class LinearBasis {
        u64 b[64] = {};
        int rank_ = 0;
        bool has_zero = false;   // some non-empty subset XORs to 0

    public:
        bool insert(u64 x) {
            for (int i = 63; i >= 0; --i) {
                if (!((x >> i) & 1)) continue;
                if (!b[i]) { b[i] = x; ++rank_; return true; }
                x ^= b[i];
            }
            has_zero = true;
            return false;
        }

        bool contains(u64 x) const {
            for (int i = 63; i >= 0; --i)
                if ((x >> i) & 1) { if (!b[i]) return false; x ^= b[i]; }
            return true;
        }

        u64 max_xor(u64 start = 0) const {
            u64 r = start;
            for (int i = 63; i >= 0; --i) if ((r ^ b[i]) > r) r ^= b[i];
            return r;
        }

        int rank() const { return rank_; }

        // k-th smallest (0-indexed) among DISTINCT XORs of non-empty subsets.
        // Returns false if k is out of range.
        bool kth_smallest(u64 k, u64& out) const {
            std::vector<u64> rows;
            u64 r[64];
            std::copy(b, b + 64, r);
            for (int i = 63; i >= 0; --i)
                if (r[i]) for (int j = i - 1; j >= 0; --j) if (r[j] && ((r[i] >> j) & 1)) r[i] ^= r[j];
            for (int i = 0; i < 64; ++i) if (r[i]) rows.push_back(r[i]);
            if (!has_zero) ++k;   // 0 is not attainable by a non-empty subset; skip it
            if (rows.size() < 64 && k >= (1ULL << rows.size())) return false;
            out = 0;
            for (size_t i = 0; i < rows.size(); ++i) if ((k >> i) & 1) out ^= rows[i];
            return true;
        }
    };

} // namespace xor_basis

int main() {
    using namespace xor_basis;
    std::cout << "============================================================\n";
    std::cout << "     XOR Linear Basis (Gaussian Elimination over GF(2))\n";
    std::cout << "============================================================\n\n";

    std::vector<u64> nums = {3, 10, 5, 25, 2, 8};
    LinearBasis lb;
    for (u64 x : nums) lb.insert(x);
    std::cout << "Numbers: 3 10 5 25 2 8\n";
    std::cout << "Rank = " << lb.rank() << ", distinct subset XORs = " << (1ULL << lb.rank()) << "\n";
    std::cout << "Maximum subset XOR = " << lb.max_xor() << "\n";
    std::cout << "Smallest 8 subset XORs: ";
    for (u64 k = 0; k < 8; ++k) { u64 v; if (lb.kth_smallest(k, v)) std::cout << v << " "; }
    std::cout << "\ncontains(26)? " << (lb.contains(26) ? "yes" : "no")
              << ", contains(64)? " << (lb.contains(64) ? "yes" : "no") << "\n";

    // Brute-force verification on random small sets
    std::mt19937_64 rng(2024);
    bool ok = true;
    for (int trial = 0; trial < 300 && ok; ++trial) {
        int n = 1 + rng() % 12;
        std::vector<u64> a(n);
        for (auto& x : a) x = rng() % 1024;
        LinearBasis basis;
        for (u64 x : a) basis.insert(x);
        std::set<u64> all;
        for (int mask = 1; mask < (1 << n); ++mask) {
            u64 x = 0;
            for (int i = 0; i < n; ++i) if (mask >> i & 1) x ^= a[i];
            all.insert(x);
        }
        if (basis.max_xor() != *all.rbegin()) ok = false;
        u64 k = 0;
        for (u64 v : all) { u64 got; if (!basis.kth_smallest(k++, got) || got != v) { ok = false; break; } }
        u64 extra;
        if (basis.kth_smallest(all.size(), extra)) ok = false;
        for (u64 q = 0; q < 1024; ++q) if (basis.contains(q) != (q == 0 || all.count(q))) { ok = false; break; }
    }
    std::cout << "\nRandom verification vs subset enumeration (max, k-th, contains): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
