/*
 * Program Description:
 * 2D Fenwick Tree (Binary Indexed Tree) in C++.
 * Extends the 1D Fenwick tree to two dimensions for efficient:
 * 1. Point updates: add value val to cell (r, c) in O(log R * log C)
 * 2. Range queries: sum of submatrix [r1..r2, c1..c2] in O(log R * log C)
 *
 * Mathematical principle:
 * 2D Inclusion-Exclusion Principle:
 * RangeSum(r1..r2, c1..c2) = PrefixSum(r2, c2)
 *                          - PrefixSum(r1 - 1, c2)
 *                          - PrefixSum(r2, c1 - 1)
 *                          + PrefixSum(r1 - 1, c1 - 1)
 *
 * Coordinates are 1-indexed internally.
 */

#include <iostream>
#include <vector>
#include <iomanip>

class FenwickTree2D {
    int rows, cols;
    std::vector<std::vector<long long>> bit;

public:
    FenwickTree2D(int r, int c) : rows(r), cols(c) {
        bit.assign(rows + 1, std::vector<long long>(cols + 1, 0));
    }

    // Point update: adds delta to cell (r, c)
    void update(int r, int c, long long delta) {
        for (int i = r; i <= rows; i += (i & -i)) {
            for (int j = c; j <= cols; j += (j & -j)) {
                bit[i][j] += delta;
            }
        }
    }

    // Prefix sum of submatrix [1..r, 1..c]
    long long queryPrefix(int r, int c) const {
        if (r <= 0 || c <= 0) return 0;
        long long sum = 0;
        for (int i = r; i > 0; i -= (i & -i)) {
            for (int j = c; j > 0; j -= (j & -j)) {
                sum += bit[i][j];
            }
        }
        return sum;
    }

    // 2D Range sum query for subgrid [r1..r2, c1..c2]
    long long queryRange(int r1, int c1, int r2, int c2) const {
        if (r1 > r2 || c1 > c2) return 0;
        return queryPrefix(r2, c2)
             - queryPrefix(r1 - 1, c2)
             - queryPrefix(r2, c1 - 1)
             + queryPrefix(r1 - 1, c1 - 1);
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "        2D Fenwick Tree (BIT) Demo        \n";
    std::cout << "==========================================\n\n";

    int r = 4, c = 4;
    FenwickTree2D bit2d(r, c);

    // Initial grid values (1-indexed):
    // 1  2  3  4
    // 5  6  7  8
    // 9 10 11 12
    // 13 14 15 16
    std::vector<std::vector<long long>> grid = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}
    };

    std::cout << "Initializing 4x4 Grid:\n";
    for (int i = 1; i <= r; ++i) {
        std::cout << "  ";
        for (int j = 1; j <= c; ++j) {
            bit2d.update(i, j, grid[i - 1][j - 1]);
            std::cout << std::setw(4) << grid[i - 1][j - 1];
        }
        std::cout << "\n";
    }

    // Query 1: Full grid sum [1..4, 1..4] = 16 * 17 / 2 = 136
    std::cout << "\nQuery Range [1..4, 1..4] sum: " << bit2d.queryRange(1, 1, 4, 4)
              << " (Expected: 136)\n";

    // Query 2: Central 2x2 subgrid [2..3, 2..3] = 6 + 7 + 10 + 11 = 34
    std::cout << "Query Subgrid [2..3, 2..3] sum: " << bit2d.queryRange(2, 2, 3, 3)
              << " (Expected: 34)\n";

    // Update: Add 10 to cell (2, 2)
    std::cout << "\nUpdating cell (2, 2) by adding +10...\n";
    bit2d.update(2, 2, 10);

    // Query 3: Central 2x2 subgrid [2..3, 2..3] should now be 34 + 10 = 44
    std::cout << "Query Subgrid [2..3, 2..3] sum: " << bit2d.queryRange(2, 2, 3, 3)
              << " (Expected: 44)\n";

    // Query 4: Full grid sum should now be 136 + 10 = 146
    std::cout << "Query Range [1..4, 1..4] sum: " << bit2d.queryRange(1, 1, 4, 4)
              << " (Expected: 146)\n";

    return 0;
}
