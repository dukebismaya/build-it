/*
 * ============================================================================
 *  Disjoint Sparse Table — O(1) Range Minimum/Maximum Queries
 * ============================================================================
 *
 *  A Disjoint Sparse Table (DST) answers arbitrary associative range queries
 *  in O(1) time per query with O(N log N) preprocessing. Unlike a standard
 *  sparse table which requires the operator to be idempotent (min/max/gcd),
 *  a DST works with ANY associative operator (sum, product, XOR, matrix
 *  multiply, etc.).
 *
 *  Construction:
 *  - Divide the array into blocks of size 2^k at each level k.
 *  - At each level, for each block, compute suffix-aggregates (left half)
 *    and prefix-aggregates (right half) toward the block center.
 *  - Total O(N log N) preprocessing.
 *
 *  Query [l, r]:
 *  - If l == r, return arr[l].
 *  - Otherwise, find the highest bit where l and r differ (using
 *    MSB of l XOR r). This identifies the level k and the block.
 *  - The answer is combine(suffix[k][l], prefix[k][r]).
 *  - This is O(1) per query.
 *
 *  Time Complexity:
 *  - Build: O(N log N)
 *  - Query: O(1) per query
 *  Space Complexity: O(N log N)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <climits>

namespace disjoint_sparse_table {

template <typename T, typename Op>
class DST {
public:
    DST(const std::vector<T>& arr, Op op, T identity)
        : n(static_cast<int>(arr.size())), data(arr), combine(op), id(identity) {
        build();
    }

    T query(int l, int r) const {
        if (l == r) return data[l];
        int k = msb(l ^ r);
        return combine(suffix[k][l], prefix[k][r]);
    }

private:
    int n;
    std::vector<T> data;
    Op combine;
    T id;

    /* suffix[k][i] = aggregate from i to center of block at level k */
    /* prefix[k][i] = aggregate from center+1 of block at level k to i */
    std::vector<std::vector<T>> prefix, suffix;

    int msb(int x) const {
        int r = 0;
        while ((1 << (r + 1)) <= x) ++r;
        return r;
    }

    void build() {
        int levels = 1;
        while ((1 << levels) < n) ++levels;

        prefix.resize(levels);
        suffix.resize(levels);

        for (int k = 0; k < levels; ++k) {
            prefix[k].assign(n, id);
            suffix[k].assign(n, id);

            int blockSize = 1 << (k + 1);
            for (int start = 0; start < n; start += blockSize) {
                int mid = std::min(start + (blockSize >> 1), n);
                int end = std::min(start + blockSize, n);

                /* Suffix aggregates for left half: from mid-1 down to start */
                if (mid - 1 >= start) {
                    suffix[k][mid - 1] = data[mid - 1];
                    for (int i = mid - 2; i >= start; --i) {
                        suffix[k][i] = combine(data[i], suffix[k][i + 1]);
                    }
                }

                /* Prefix aggregates for right half: from mid up to end-1 */
                if (mid < end) {
                    prefix[k][mid] = data[mid];
                    for (int i = mid + 1; i < end; ++i) {
                        prefix[k][i] = combine(prefix[k][i - 1], data[i]);
                    }
                }
            }
        }
    }
};

/* Brute force for verification */
template <typename T, typename Op>
T bruteQuery(const std::vector<T>& arr, int l, int r, Op op) {
    T result = arr[l];
    for (int i = l + 1; i <= r; ++i) {
        result = op(result, arr[i]);
    }
    return result;
}

} // namespace disjoint_sparse_table

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Disjoint Sparse Table — O(1) Associative Range Queries\n";
    std::cout << "========================================================\n\n";

    std::vector<int> arr = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9};
    int n = static_cast<int>(arr.size());

    std::cout << "Array (" << n << " elements): [";
    for (int i = 0; i < n; ++i) {
        std::cout << arr[i] << (i + 1 < n ? ", " : "]\n\n");
    }

    /* Test with SUM */
    auto addOp = [](int a, int b) { return a + b; };
    disjoint_sparse_table::DST<int, decltype(addOp)> sumDST(arr, addOp, 0);

    /* Test with MIN */
    auto minOp = [](int a, int b) { return std::min(a, b); };
    disjoint_sparse_table::DST<int, decltype(minOp)> minDST(arr, minOp, INT_MAX);

    /* Test with XOR */
    auto xorOp = [](int a, int b) { return a ^ b; };
    disjoint_sparse_table::DST<int, decltype(xorOp)> xorDST(arr, xorOp, 0);

    struct QueryRange { int l, r; };
    std::vector<QueryRange> queries = {
        {0, 0}, {0, 4}, {3, 9}, {0, 14}, {5, 12}, {7, 13}, {2, 2}, {6, 10}
    };

    std::cout << "Range SUM Queries (O(1) per query):\n";
    std::cout << std::setw(10) << "[L, R]" << std::setw(10) << "DST"
              << std::setw(10) << "Brute" << std::setw(10) << "Match?" << "\n";
    std::cout << "----------------------------------------\n";

    bool allMatch = true;
    for (auto& q : queries) {
        int dst = sumDST.query(q.l, q.r);
        int brute = disjoint_sparse_table::bruteQuery(arr, q.l, q.r, addOp);
        bool match = (dst == brute);
        if (!match) allMatch = false;

        std::string range = "[" + std::to_string(q.l) + "," + std::to_string(q.r) + "]";
        std::cout << std::setw(10) << range << std::setw(10) << dst
                  << std::setw(10) << brute << std::setw(10) << (match ? "YES" : "NO") << "\n";
    }

    std::cout << "\nRange MIN Queries:\n";
    std::cout << std::setw(10) << "[L, R]" << std::setw(10) << "DST"
              << std::setw(10) << "Brute" << std::setw(10) << "Match?" << "\n";
    std::cout << "----------------------------------------\n";

    for (auto& q : queries) {
        int dst = minDST.query(q.l, q.r);
        int brute = disjoint_sparse_table::bruteQuery(arr, q.l, q.r, minOp);
        bool match = (dst == brute);
        if (!match) allMatch = false;

        std::string range = "[" + std::to_string(q.l) + "," + std::to_string(q.r) + "]";
        std::cout << std::setw(10) << range << std::setw(10) << dst
                  << std::setw(10) << brute << std::setw(10) << (match ? "YES" : "NO") << "\n";
    }

    std::cout << "\nRange XOR Queries:\n";
    std::cout << std::setw(10) << "[L, R]" << std::setw(10) << "DST"
              << std::setw(10) << "Brute" << std::setw(10) << "Match?" << "\n";
    std::cout << "----------------------------------------\n";

    for (auto& q : queries) {
        int dst = xorDST.query(q.l, q.r);
        int brute = disjoint_sparse_table::bruteQuery(arr, q.l, q.r, xorOp);
        bool match = (dst == brute);
        if (!match) allMatch = false;

        std::string range = "[" + std::to_string(q.l) + "," + std::to_string(q.r) + "]";
        std::cout << std::setw(10) << range << std::setw(10) << dst
                  << std::setw(10) << brute << std::setw(10) << (match ? "YES" : "NO") << "\n";
    }

    std::cout << "\nAll queries verified: " << (allMatch ? "PASSED" : "FAILED") << "\n";

    return 0;
}
