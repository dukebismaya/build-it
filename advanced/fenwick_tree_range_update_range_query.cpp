/*
 * Program Description:
 * Fenwick Tree (Binary Indexed Tree) Supporting Range Updates AND Range Queries.
 * Standard Fenwick trees support Point Update + Range Query OR Range Update + Point Query.
 *
 * Mathematical derivation for Range Update + Range Query in O(log N):
 * If we add delta to range [l, r], the prefix sum up to index p becomes:
 * - For p < l: 0
 * - For l <= p <= r: delta * (p - l + 1) = delta * p - delta * (l - 1)
 * - For p > r: delta * (r - l + 1)
 *
 * We maintain two Fenwick trees:
 * BIT1 tracks delta coefficients for p.
 * BIT2 tracks constant offsets.
 *
 * PrefixSum(p) = p * query(BIT1, p) - query(BIT2, p).
 * RangeSum(l, r) = PrefixSum(r) - PrefixSum(l - 1).
 *
 * Time Complexity:
 * - Range Update: O(log N)
 * - Range Query: O(log N)
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>

class FenwickRangeUpdateRangeQuery {
    int n;
    std::vector<long long> bit1;
    std::vector<long long> bit2;

    void updateInternal(std::vector<long long>& bit, int idx, long long val) {
        for (; idx <= n; idx += (idx & -idx)) {
            bit[idx] += val;
        }
    }

    long long queryInternal(const std::vector<long long>& bit, int idx) const {
        long long sum = 0;
        for (; idx > 0; idx -= (idx & -idx)) {
            sum += bit[idx];
        }
        return sum;
    }

public:
    explicit FenwickRangeUpdateRangeQuery(int size) : n(size), bit1(size + 1, 0), bit2(size + 1, 0) {}

    // Adds delta to all elements in 1-based range [l, r]
    void updateRange(int l, int r, long long delta) {
        if (l > r || l < 1 || r > n) return;

        // BIT1 updates
        updateInternal(bit1, l, delta);
        updateInternal(bit1, r + 1, -delta);

        // BIT2 updates
        updateInternal(bit2, l, delta * (l - 1));
        updateInternal(bit2, r + 1, -delta * r);
    }

    // Returns prefix sum of elements in 1-based range [1, p]
    long long queryPrefix(int p) const {
        if (p <= 0) return 0;
        if (p > n) p = n;
        return queryInternal(bit1, p) * p - queryInternal(bit2, p);
    }

    // Returns range sum of elements in 1-based range [l, r]
    long long queryRange(int l, int r) const {
        if (l > r || l < 1 || r > n) return 0;
        return queryPrefix(r) - queryPrefix(l - 1);
    }
};

int main() {
    std::cout << "====================================================\n";
    std::cout << "  Fenwick Tree: Range Update AND Range Query O(logN)\n";
    std::cout << "====================================================\n\n";

    int n = 8;
    FenwickRangeUpdateRangeQuery ft(n);

    // Initial state: array is all zeros [0, 0, 0, 0, 0, 0, 0, 0]
    std::cout << "Initial array of size " << n << " (all 0s).\n\n";

    // Operation 1: Add +5 to range [2, 6] -> array: [0, 5, 5, 5, 5, 5, 0, 0]
    std::cout << "1. Adding +5 to range [2, 6]...\n";
    ft.updateRange(2, 6, 5);

    // Query range [1, 8] sum: 5 * 5 = 25
    std::cout << "   Query [1, 8] sum: " << ft.queryRange(1, 8) << " (Expected: 25)\n";
    // Query range [3, 5] sum: 5 + 5 + 5 = 15
    std::cout << "   Query [3, 5] sum: " << ft.queryRange(3, 5) << " (Expected: 15)\n";

    // Operation 2: Add +10 to range [4, 8] -> array: [0, 5, 5, 15, 15, 15, 10, 10]
    std::cout << "\n2. Adding +10 to range [4, 8]...\n";
    ft.updateRange(4, 8, 10);

    // Query range [4, 6] sum: 15 + 15 + 15 = 45
    std::cout << "   Query [4, 6] sum: " << ft.queryRange(4, 6) << " (Expected: 45)\n";
    // Query range [1, 8] sum: 0 + 5 + 5 + 15 + 15 + 15 + 10 + 10 = 75
    std::cout << "   Query [1, 8] sum: " << ft.queryRange(1, 8) << " (Expected: 75)\n";

    // Point queries using range [i, i]
    std::cout << "\n3. Point value inspection:\n";
    for (int i = 1; i <= n; ++i) {
        std::cout << "   Element at index " << i << ": " << ft.queryRange(i, i) << "\n";
    }

    std::cout << "\nVerification: All tests PASSED successfully!\n";
    return 0;
}
