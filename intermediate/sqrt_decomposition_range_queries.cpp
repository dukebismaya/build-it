/*
 * ============================================================
 *      Square Root Decomposition (Range Add / Range Sum / Min)
 * ============================================================
 *
 * Description:
 *   Split an array of N elements into blocks of size B ~ sqrt(N).
 *   For each block keep:
 *     sum[b]  - sum of the block's elements
 *     mn[b]   - minimum of the block's elements
 *     lazy[b] - pending value added to the whole block
 *
 *   range_add(l, r, v):
 *     - partial blocks at both ends: update elements, rebuild
 *     - full blocks in between: lazy[b] += v (O(1) each)
 *   range_sum / range_min(l, r):
 *     - partial ends: scan elements (+ lazy)
 *     - full blocks: use the block aggregates
 *
 * Complexity:
 *   Each operation O(sqrt N); build O(N); space O(N).
 *   Simple, cache-friendly alternative to a lazy segment tree.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <climits>
#include <random>
#include <algorithm>

namespace sqrt_decomp {

    class SqrtArray {
        int n, B;
        std::vector<long long> a, sum, mn, lazy;

        void rebuild(int b) {
            int lo = b * B, hi = std::min(n, lo + B);
            sum[b] = 0; mn[b] = LLONG_MAX;
            for (int i = lo; i < hi; ++i) { sum[b] += a[i]; mn[b] = std::min(mn[b], a[i]); }
        }

    public:
        explicit SqrtArray(const std::vector<long long>& init)
            : n(static_cast<int>(init.size())), B(std::max(1, static_cast<int>(std::sqrt(init.size())))),
              a(init) {
            int blocks = (n + B - 1) / B;
            sum.assign(blocks, 0); mn.assign(blocks, LLONG_MAX); lazy.assign(blocks, 0);
            for (int b = 0; b < blocks; ++b) rebuild(b);
        }

        void range_add(int l, int r, long long v) {
            int bl = l / B, br = r / B;
            if (bl == br) {
                for (int i = l; i <= r; ++i) a[i] += v;
                rebuild(bl);
                return;
            }
            for (int i = l; i < (bl + 1) * B; ++i) a[i] += v;
            rebuild(bl);
            for (int b = bl + 1; b < br; ++b) lazy[b] += v;
            for (int i = br * B; i <= r; ++i) a[i] += v;
            rebuild(br);
        }

        long long range_sum(int l, int r) const {
            int bl = l / B, br = r / B;
            long long s = 0;
            if (bl == br) {
                for (int i = l; i <= r; ++i) s += a[i] + lazy[bl];
                return s;
            }
            for (int i = l; i < (bl + 1) * B; ++i) s += a[i] + lazy[bl];
            for (int b = bl + 1; b < br; ++b) s += sum[b] + lazy[b] * B;
            for (int i = br * B; i <= r; ++i) s += a[i] + lazy[br];
            return s;
        }

        long long range_min(int l, int r) const {
            int bl = l / B, br = r / B;
            long long m = LLONG_MAX;
            if (bl == br) {
                for (int i = l; i <= r; ++i) m = std::min(m, a[i] + lazy[bl]);
                return m;
            }
            for (int i = l; i < (bl + 1) * B; ++i) m = std::min(m, a[i] + lazy[bl]);
            for (int b = bl + 1; b < br; ++b) m = std::min(m, mn[b] + lazy[b]);
            for (int i = br * B; i <= r; ++i) m = std::min(m, a[i] + lazy[br]);
            return m;
        }

        int block_size() const { return B; }
    };

} // namespace sqrt_decomp

int main() {
    std::cout << "============================================================\n";
    std::cout << "   Square Root Decomposition (Range Add / Sum / Min)\n";
    std::cout << "============================================================\n\n";

    std::vector<long long> init = {5, 2, 8, 6, 3, 7, 1, 9, 4, 10};
    sqrt_decomp::SqrtArray sa(init);
    std::cout << "Array: 5 2 8 6 3 7 1 9 4 10  (block size " << sa.block_size() << ")\n";
    std::cout << "sum[2..7] = " << sa.range_sum(2, 7) << ", min[2..7] = " << sa.range_min(2, 7) << "\n";
    sa.range_add(1, 8, 3);
    std::cout << "After add +3 on [1..8]: sum[0..9] = " << sa.range_sum(0, 9)
              << ", min[0..9] = " << sa.range_min(0, 9) << "\n";

    std::mt19937 rng(17);
    const int N = 5000;
    std::vector<long long> ref(N);
    for (auto& x : ref) x = static_cast<long long>(rng() % 2001) - 1000;
    sqrt_decomp::SqrtArray big(ref);
    bool ok = true;
    for (int q = 0; q < 20000 && ok; ++q) {
        int l = rng() % N, r = rng() % N;
        if (l > r) std::swap(l, r);
        int t = rng() % 3;
        if (t == 0) {
            long long v = static_cast<long long>(rng() % 201) - 100;
            big.range_add(l, r, v);
            for (int i = l; i <= r; ++i) ref[i] += v;
        } else if (t == 1) {
            long long s = 0; for (int i = l; i <= r; ++i) s += ref[i];
            if (s != big.range_sum(l, r)) ok = false;
        } else {
            long long m = *std::min_element(ref.begin() + l, ref.begin() + r + 1);
            if (m != big.range_min(l, r)) ok = false;
        }
    }
    std::cout << "\nRandom test vs brute force (N=5000, 20000 ops): " << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
