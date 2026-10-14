/*
 * Problem Description:
 * Sparse Table for Range Minimum Query (RMQ).
 * A data structure that pre-processes an array of size N to answer range minimum
 * (or maximum) queries in O(1) time after O(N log N) preprocessing.
 * Sparse tables exploit the overlap-friendly property of idempotent functions (min, max, gcd):
 * min(a, a) = a, so overlapping ranges don't affect the result.
 * Builds a 2D table where table[i][j] stores the answer for the range starting at index i
 * with length 2^j.
 * Not suitable for mutable arrays (use Segment Tree for that).
 * Build Time: O(N log N)
 * Query Time: O(1)
 * Space: O(N log N)
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

class SparseTable {
    std::vector<std::vector<int>> table;
    std::vector<int> logTable;
    int n;

public:
    SparseTable(const std::vector<int>& arr) {
        n = arr.size();
        int maxLog = static_cast<int>(std::log2(n)) + 1;
        table.assign(n, std::vector<int>(maxLog + 1, 0));
        logTable.resize(n + 1);

        // Precompute logs
        logTable[1] = 0;
        for (int i = 2; i <= n; ++i) {
            logTable[i] = logTable[i / 2] + 1;
        }

        // Initialize for range length 1
        for (int i = 0; i < n; ++i) {
            table[i][0] = arr[i];
        }

        // Build sparse table
        for (int j = 1; (1 << j) <= n; ++j) {
            for (int i = 0; i + (1 << j) - 1 < n; ++i) {
                table[i][j] = std::min(table[i][j - 1], table[i + (1 << (j - 1))][j - 1]);
            }
        }
    }

    // O(1) range minimum query on [l, r] (0-indexed, inclusive)
    int query(int l, int r) const {
        int length = r - l + 1;
        int k = logTable[length];
        return std::min(table[l][k], table[r - (1 << k) + 1][k]);
    }
};

int main() {
    std::vector<int> arr = {7, 2, 3, 0, 5, 10, 3, 12, 18, 1};

    SparseTable st(arr);

    std::cout << "Sparse Table - Range Minimum Query (RMQ)\n\n";
    std::cout << "Array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << "\n\n";

    struct QueryTest { int l, r; };
    QueryTest queries[] = {{0, 4}, {1, 3}, {3, 8}, {0, 9}, {5, 7}, {8, 9}};

    for (const auto& q : queries) {
        std::cout << "min(arr[" << q.l << ".." << q.r << "]) = " << st.query(q.l, q.r) << "\n";
    }

    return 0;
}
