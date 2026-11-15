/*
 * ============================================================================
 *  Fenwick Tree with Range Update and Range Query (RURQ)
 * ============================================================================
 *
 *  A standard Fenwick Tree (BIT) supports point updates and prefix sum queries.
 *  With two BITs, we can extend this to support:
 *    - Range Update: add value v to all elements in [l, r]
 *    - Range Query:  compute sum of elements in [l, r]
 *  Both in O(log N) time.
 *
 *  Derivation:
 *  After a range update add(l, r, v), the prefix sum at position x is:
 *    sum(x) = sum of all previous updates that affect [1, x]
 *
 *  For an update add(l, r, v), the contribution to prefix sum at x is:
 *    - 0,                if x < l
 *    - v * (x - l + 1),  if l <= x <= r
 *    - v * (r - l + 1),  if x > r
 *
 *  Rewrite v * (x - l + 1) = v * x - v * (l - 1).
 *  So we maintain two BITs:
 *    B1[i]: accumulates v values (coefficients of x)
 *    B2[i]: accumulates v * (l-1) or v * r values (constant terms)
 *
 *  prefix_sum(x) = B1.query(x) * x - B2.query(x)
 *
 *  Range update add(l, r, v):
 *    B1: add v at l, add -v at r+1
 *    B2: add v*(l-1) at l, add -v*r at r+1
 *
 *  Range query sum(l, r) = prefix_sum(r) - prefix_sum(l-1)
 *
 *  Time Complexity:  O(log N) per update and query
 *  Space Complexity: O(N)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

namespace fenwick_rurq {

class BIT {
public:
    BIT(int n) : size(n), tree(n + 1, 0) {}

    void update(int i, int64_t delta) {
        for (; i <= size; i += i & (-i)) {
            tree[i] += delta;
        }
    }

    int64_t query(int i) const {
        int64_t sum = 0;
        for (int x = i; x > 0; x -= x & (-x)) {
            sum += tree[x];
        }
        return sum;
    }

private:
    int size;
    std::vector<int64_t> tree;
};

class RangeUpdateRangeQuery {
public:
    RangeUpdateRangeQuery(int n) : size(n), b1(n), b2(n) {}

    /* Add value v to all elements in [l, r] (1-indexed) */
    void rangeUpdate(int l, int r, int64_t v) {
        b1.update(l, v);
        b1.update(r + 1, -v);
        b2.update(l, v * (l - 1));
        b2.update(r + 1, -v * r);
    }

    /* Prefix sum [1, x] */
    int64_t prefixSum(int x) const {
        return b1.query(x) * x - b2.query(x);
    }

    /* Range sum [l, r] */
    int64_t rangeSum(int l, int r) const {
        return prefixSum(r) - prefixSum(l - 1);
    }

private:
    int size;
    BIT b1, b2;
};

/* Brute force for verification */
class BruteForce {
public:
    BruteForce(int n) : arr(n + 1, 0) {}

    void rangeUpdate(int l, int r, int64_t v) {
        for (int i = l; i <= r; ++i) {
            arr[i] += v;
        }
    }

    int64_t rangeSum(int l, int r) const {
        int64_t sum = 0;
        for (int i = l; i <= r; ++i) {
            sum += arr[i];
        }
        return sum;
    }

private:
    std::vector<int64_t> arr;
};

} // namespace fenwick_rurq

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Fenwick Tree — Range Update & Range Query (RURQ)\n";
    std::cout << "========================================================\n\n";

    int n = 10;
    fenwick_rurq::RangeUpdateRangeQuery ft(n);
    fenwick_rurq::BruteForce bf(n);

    struct Operation {
        char type;  /* 'U' = update, 'Q' = query */
        int l, r;
        int64_t v;  /* value for update, unused for query */
    };

    std::vector<Operation> ops = {
        {'U', 1, 5, 3},    /* Add 3 to [1,5] */
        {'U', 3, 8, 2},    /* Add 2 to [3,8] */
        {'Q', 1, 10, 0},   /* Sum [1,10] */
        {'Q', 3, 5, 0},    /* Sum [3,5] */
        {'U', 6, 10, -1},  /* Add -1 to [6,10] */
        {'Q', 1, 5, 0},    /* Sum [1,5] */
        {'Q', 6, 10, 0},   /* Sum [6,10] */
        {'U', 1, 10, 1},   /* Add 1 to [1,10] */
        {'Q', 1, 1, 0},    /* Sum [1,1] */
        {'Q', 1, 10, 0},   /* Sum [1,10] */
        {'U', 4, 4, 100},  /* Point update at 4 */
        {'Q', 3, 5, 0},    /* Sum [3,5] */
        {'Q', 4, 4, 0},    /* Point query at 4 */
    };

    std::cout << "Operations on array of size " << n << " (1-indexed, init to 0):\n\n";
    std::cout << std::setw(6) << "Op#" << std::setw(8) << "Type"
              << std::setw(10) << "[L, R]" << std::setw(8) << "Val"
              << std::setw(14) << "Fenwick" << std::setw(12) << "Brute"
              << std::setw(10) << "Match?" << "\n";
    std::cout << "--------------------------------------------------------------\n";

    bool allMatch = true;
    int opNum = 0;

    for (auto& op : ops) {
        ++opNum;
        std::string range = "[" + std::to_string(op.l) + "," + std::to_string(op.r) + "]";

        if (op.type == 'U') {
            ft.rangeUpdate(op.l, op.r, op.v);
            bf.rangeUpdate(op.l, op.r, op.v);
            std::cout << std::setw(6) << opNum << std::setw(8) << "UPDATE"
                      << std::setw(10) << range << std::setw(8) << op.v
                      << std::setw(14) << "---" << std::setw(12) << "---"
                      << std::setw(10) << "---" << "\n";
        } else {
            int64_t ftResult = ft.rangeSum(op.l, op.r);
            int64_t bfResult = bf.rangeSum(op.l, op.r);
            bool match = (ftResult == bfResult);
            if (!match) allMatch = false;

            std::cout << std::setw(6) << opNum << std::setw(8) << "QUERY"
                      << std::setw(10) << range << std::setw(8) << "---"
                      << std::setw(14) << ftResult << std::setw(12) << bfResult
                      << std::setw(10) << (match ? "YES" : "NO") << "\n";
        }
    }

    std::cout << "--------------------------------------------------------------\n";
    std::cout << "All queries verified: " << (allMatch ? "PASSED" : "FAILED") << "\n";

    /* Show final array state */
    std::cout << "\nFinal array state (via point queries):\n  [";
    for (int i = 1; i <= n; ++i) {
        std::cout << ft.rangeSum(i, i) << (i < n ? ", " : "]\n");
    }

    return 0;
}
