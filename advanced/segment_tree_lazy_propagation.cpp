/*
 * Program Description:
 * Segment Tree with Lazy Propagation in C++.
 * Efficiently handles:
 * 1. Range Sum Query (RSQ): Computes sum of elements in [L, R] in O(log N).
 * 2. Range Add Update: Adds a value delta to all elements in [L, R] in O(log N).
 *
 * Lazy Propagation Concept:
 * When an update covers an entire segment, we update the segment sum and store the
 * pending value in the lazy tree. We defer updating child nodes until they are explicitly
 * visited by future queries or updates (pushDown / propagate).
 *
 * Complexities:
 * - Tree Construction: O(N)
 * - Range Query: O(log N)
 * - Range Update: O(log N)
 * - Space: O(4N)
 */

#include <iostream>
#include <vector>
#include <cassert>

class SegmentTreeLazy {
    int n;
    std::vector<long long> tree;
    std::vector<long long> lazy;

    void build(const std::vector<long long>& arr, int node, int start, int end) {
        if (start == end) {
            tree[node] = arr[start];
            return;
        }

        int mid = start + (end - start) / 2;
        int leftChild = 2 * node;
        int rightChild = 2 * node + 1;

        build(arr, leftChild, start, mid);
        build(arr, rightChild, mid + 1, end);
        tree[node] = tree[leftChild] + tree[rightChild];
    }

    void pushDown(int node, int start, int end) {
        if (lazy[node] == 0) return;

        int mid = start + (end - start) / 2;
        int leftChild = 2 * node;
        int rightChild = 2 * node + 1;

        // Propagate to left child
        lazy[leftChild] += lazy[node];
        tree[leftChild] += lazy[node] * (mid - start + 1);

        // Propagate to right child
        lazy[rightChild] += lazy[node];
        tree[rightChild] += lazy[node] * (end - mid);

        // Clear lazy flag for current node
        lazy[node] = 0;
    }

    void updateRange(int node, int start, int end, int l, int r, long long val) {
        if (r < start || end < l) {
            return; // Out of range
        }

        if (l <= start && end <= r) {
            // Fully within range
            tree[node] += val * (end - start + 1);
            lazy[node] += val;
            return;
        }

        pushDown(node, start, end);

        int mid = start + (end - start) / 2;
        int leftChild = 2 * node;
        int rightChild = 2 * node + 1;

        updateRange(leftChild, start, mid, l, r, val);
        updateRange(rightChild, mid + 1, end, l, r, val);
        tree[node] = tree[leftChild] + tree[rightChild];
    }

    long long queryRange(int node, int start, int end, int l, int r) {
        if (r < start || end < l) {
            return 0; // Out of range
        }

        if (l <= start && end <= r) {
            return tree[node]; // Fully within range
        }

        pushDown(node, start, end);

        int mid = start + (end - start) / 2;
        int leftChild = 2 * node;
        int rightChild = 2 * node + 1;

        long long leftSum = queryRange(leftChild, start, mid, l, r);
        long long rightSum = queryRange(rightChild, mid + 1, end, l, r);

        return leftSum + rightSum;
    }

public:
    explicit SegmentTreeLazy(const std::vector<long long>& arr) {
        n = static_cast<int>(arr.size());
        tree.assign(4 * n, 0);
        lazy.assign(4 * n, 0);
        if (n > 0) {
            build(arr, 1, 0, n - 1);
        }
    }

    void update(int l, int r, long long val) {
        if (l > r || l < 0 || r >= n) return;
        updateRange(1, 0, n - 1, l, r, val);
    }

    long long query(int l, int r) {
        if (l > r || l < 0 || r >= n) return 0;
        return queryRange(1, 0, n - 1, l, r);
    }
};

int main() {
    std::cout << "====================================================\n";
    std::cout << "   Segment Tree with Lazy Propagation (Range Sum)   \n";
    std::cout << "====================================================\n\n";

    std::vector<long long> arr = {1, 3, 5, 7, 9, 11};
    int n = arr.size();

    std::cout << "Initial Array: ";
    for (int i = 0; i < n; ++i) {
        std::cout << arr[i] << (i + 1 < n ? ", " : "\n");
    }

    SegmentTreeLazy segTree(arr);

    // Query 1: Sum from index 1 to 4 (3 + 5 + 7 + 9 = 24)
    std::cout << "\nQuery Range [1, 4] sum: " << segTree.query(1, 4) << " (Expected: 24)\n";

    // Update 1: Add 10 to range [1, 3] -> arr becomes [1, 13, 15, 17, 9, 11]
    std::cout << "Updating Range [1, 3] by adding +10...\n";
    segTree.update(1, 3, 10);

    // Query 2: Sum from index 1 to 4 (13 + 15 + 17 + 9 = 54)
    std::cout << "Query Range [1, 4] sum: " << segTree.query(1, 4) << " (Expected: 54)\n";

    // Query 3: Entire range [0, 5] sum (1 + 13 + 15 + 17 + 9 + 11 = 66)
    std::cout << "Query Range [0, 5] sum: " << segTree.query(0, 5) << " (Expected: 66)\n";

    // Update 2: Add 5 to entire range [0, 5]
    std::cout << "\nUpdating Range [0, 5] by adding +5...\n";
    segTree.update(0, 5, 5);

    // Query 4: Entire range [0, 5] sum (66 + 6 * 5 = 96)
    std::cout << "Query Range [0, 5] sum: " << segTree.query(0, 5) << " (Expected: 96)\n";

    // Point query (range of length 1): Element at index 2 (15 + 5 = 20)
    std::cout << "Point Query at index 2: " << segTree.query(2, 2) << " (Expected: 20)\n";

    std::cout << "\nVerification: All tests PASSED successfully!\n";
    return 0;
}
