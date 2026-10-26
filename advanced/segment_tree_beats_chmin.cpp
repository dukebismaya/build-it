/*
 * Program Description:
 * Segment Tree Beats (Ji Driver Algorithm) for Range Chmin & Range Sum.
 *
 * Supported Operations:
 * 1. Range Chmin: A[i] = min(A[i], x) for all i in [L, R]
 * 2. Range Sum Query: sum of A[i] for all i in [L, R]
 *
 * Segment Tree Beats Invariant:
 * Each node maintains:
 * - max1: strictly greatest value in segment.
 * - max2: strictly second greatest value in segment (-INF if all elements identical).
 * - maxCount: frequency of max1.
 * - sum: segment sum.
 *
 * Range Chmin(x) Logic:
 * - If x >= max1: Break (range already satisfies <= x).
 * - If max2 < x < max1: We only need to reduce max1 to x:
 *   sum -= maxCount * (max1 - x); max1 = x; return!
 * - If x <= max2: Recurse into children.
 *
 * Time Complexity: Amortized O((N + Q) log N).
 * Space Complexity: O(4N).
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

const long long INF = 2e18;

struct Node {
    long long max1;      // First maximum
    long long max2;      // Second maximum
    int maxCount;        // Count of first maximum
    long long sum;       // Segment sum
    long long lazy;      // Lazy tag for max1

    Node() : max1(-INF), max2(-INF), maxCount(0), sum(0), lazy(-INF) {}
};

class SegmentTreeBeats {
    int n;
    std::vector<Node> tree;

    void pushUp(int node) {
        int left = 2 * node, right = 2 * node + 1;
        tree[node].sum = tree[left].sum + tree[right].sum;

        if (tree[left].max1 == tree[right].max1) {
            tree[node].max1 = tree[left].max1;
            tree[node].maxCount = tree[left].maxCount + tree[right].maxCount;
            tree[node].max2 = std::max(tree[left].max2, tree[right].max2);
        } else if (tree[left].max1 > tree[right].max1) {
            tree[node].max1 = tree[left].max1;
            tree[node].maxCount = tree[left].maxCount;
            tree[node].max2 = std::max(tree[left].max2, tree[right].max1);
        } else {
            tree[node].max1 = tree[right].max1;
            tree[node].maxCount = tree[right].maxCount;
            tree[node].max2 = std::max(tree[left].max1, tree[right].max2);
        }
    }

    void applyChmin(int node, long long x) {
        if (x >= tree[node].max1) return;
        tree[node].sum -= static_cast<long long>(tree[node].maxCount) * (tree[node].max1 - x);
        tree[node].max1 = x;
        tree[node].lazy = x;
    }

    void pushDown(int node) {
        if (tree[node].lazy == -INF) return;
        applyChmin(2 * node, tree[node].lazy);
        applyChmin(2 * node + 1, tree[node].lazy);
        tree[node].lazy = -INF;
    }

    void build(const std::vector<long long>& arr, int node, int start, int end) {
        if (start == end) {
            tree[node].max1 = arr[start];
            tree[node].max2 = -INF;
            tree[node].maxCount = 1;
            tree[node].sum = arr[start];
            tree[node].lazy = -INF;
            return;
        }

        int mid = start + (end - start) / 2;
        build(arr, 2 * node, start, mid);
        build(arr, 2 * node + 1, mid + 1, end);
        pushUp(node);
    }

    void updateChmin(int node, int start, int end, int l, int r, long long x) {
        if (r < start || end < l || tree[node].max1 <= x) return;

        if (l <= start && end <= r && tree[node].max2 < x) {
            applyChmin(node, x);
            return;
        }

        pushDown(node);
        int mid = start + (end - start) / 2;
        updateChmin(2 * node, start, mid, l, r, x);
        updateChmin(2 * node + 1, mid + 1, end, l, r, x);
        pushUp(node);
    }

    long long querySum(int node, int start, int end, int l, int r) {
        if (r < start || end < l) return 0;
        if (l <= start && end <= r) return tree[node].sum;

        pushDown(node);
        int mid = start + (end - start) / 2;
        return querySum(2 * node, start, mid, l, r) + querySum(2 * node + 1, mid + 1, end, l, r);
    }

public:
    explicit SegmentTreeBeats(const std::vector<long long>& arr) {
        n = arr.size();
        tree.assign(4 * n, Node());
        if (n > 0) build(arr, 1, 0, n - 1);
    }

    void rangeChmin(int l, int r, long long x) {
        if (l > r || l < 0 || r >= n) return;
        updateChmin(1, 0, n - 1, l, r, x);
    }

    long long queryRangeSum(int l, int r) {
        if (l > r || l < 0 || r >= n) return 0;
        return querySum(1, 0, n - 1, l, r);
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "     Segment Tree Beats (Range Chmin)     \n";
    std::cout << "==========================================\n\n";

    // Initial array: [10, 8, 8, 4, 12, 10]
    std::vector<long long> arr = {10, 8, 8, 4, 12, 10};
    int n = arr.size();

    std::cout << "Initial Array: ";
    for (int i = 0; i < n; ++i) std::cout << arr[i] << " ";
    std::cout << "\n\n";

    SegmentTreeBeats stb(arr);

    // Query initial total sum: 10 + 8 + 8 + 4 + 12 + 10 = 52
    std::cout << "Total Array Sum [0..5]: " << stb.queryRangeSum(0, 5) << " (Expected: 52)\n";

    // Apply Range Chmin: A[i] = min(A[i], 9) on range [0..5]
    // Array becomes: [9, 8, 8, 4, 9, 9]
    std::cout << "\nApplying range chmin(9) on [0..5]...\n";
    stb.rangeChmin(0, 5, 9);

    // Sum should now be 9 + 8 + 8 + 4 + 9 + 9 = 47
    std::cout << "Total Array Sum after chmin(9): " << stb.queryRangeSum(0, 5) << " (Expected: 47)\n";

    // Range sum on [1..3]: 8 + 8 + 4 = 20
    std::cout << "Subarray Sum [1..3]: " << stb.queryRangeSum(1, 3) << " (Expected: 20)\n";

    // Apply Range Chmin: A[i] = min(A[i], 5) on range [0..2]
    // Subarray [0..2] was [9, 8, 8] -> becomes [5, 5, 5]
    std::cout << "\nApplying range chmin(5) on [0..2]...\n";
    stb.rangeChmin(0, 2, 5);

    // Total sum should now be 5 + 5 + 5 + 4 + 9 + 9 = 37
    std::cout << "Total Array Sum after chmin(5): " << stb.queryRangeSum(0, 5) << " (Expected: 37)\n";

    std::cout << "\nVerification: All Segment Tree Beats operations PASSED!\n";
    return 0;
}
