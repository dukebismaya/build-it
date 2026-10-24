/*
 * Program Description:
 * Persistent Segment Tree (Chairman Tree) in C++.
 * Maintains all versions of a Segment Tree over time by sharing unchanged nodes.
 * Each update creates only O(log N) new nodes rather than copying the entire tree.
 *
 * Classic Application:
 * Range k-th Smallest Element Query in subarray [L, R] in strictly O(log N) time!
 *
 * Algorithm Details:
 * 1. Coordinate compress the input array values.
 * 2. Version 0: Empty frequency segment tree.
 * 3. Version i: Insert element arr[i] into Version (i - 1).
 * 4. Query Range [L, R] for k-th smallest:
 *    - Simultaneously traverse root[R] and root[L - 1].
 *    - Number of elements in left child within range [L, R] is:
 *      count = root[R]->left->count - root[L-1]->left->count.
 *    - If k <= count, branch to left child.
 *    - Otherwise, branch to right child with k = k - count.
 *
 * Time Complexity:
 * - Build & All Updates: O(N log N)
 * - Range k-th Query: O(log N)
 * Space Complexity: O(N log N)
 */

#include <iostream>
#include <vector>
#include <algorithm>

struct Node {
    int count = 0;
    int left = 0;
    int right = 0;
};

class PersistentSegmentTree {
    int n, maxVal;
    std::vector<Node> tree;
    std::vector<int> roots;
    std::vector<int> sortedVals;

    int update(int prevRoot, int start, int end, int val) {
        int cur = tree.size();
        tree.push_back(tree[prevRoot]);
        tree[cur].count++;

        if (start == end) {
            return cur;
        }

        int mid = start + (end - start) / 2;
        if (val <= mid) {
            int newLeft = update(tree[prevRoot].left, start, mid, val);
            tree[cur].left = newLeft;
        } else {
            int newRight = update(tree[prevRoot].right, mid + 1, end, val);
            tree[cur].right = newRight;
        }

        return cur;
    }

    int queryKth(int leftNode, int rightNode, int start, int end, int k) const {
        if (start == end) {
            return start;
        }

        int countLeft = tree[tree[rightNode].left].count - tree[tree[leftNode].left].count;
        int mid = start + (end - start) / 2;

        if (k <= countLeft) {
            return queryKth(tree[leftNode].left, tree[rightNode].left, start, mid, k);
        } else {
            return queryKth(tree[leftNode].right, tree[rightNode].right, mid + 1, end, k - countLeft);
        }
    }

public:
    explicit PersistentSegmentTree(const std::vector<int>& arr) {
        n = arr.size();
        sortedVals = arr;
        std::sort(sortedVals.begin(), sortedVals.end());
        sortedVals.erase(std::unique(sortedVals.begin(), sortedVals.end()), sortedVals.end());
        maxVal = sortedVals.size();

        // Node 0 represents empty null node
        tree.emplace_back();
        roots.push_back(0); // Version 0 is empty

        for (int x : arr) {
            // Coordinate compression to [1..maxVal]
            int rank = std::lower_bound(sortedVals.begin(), sortedVals.end(), x) - sortedVals.begin() + 1;
            int newRoot = update(roots.back(), 1, maxVal, rank);
            roots.push_back(newRoot);
        }
    }

    // Queries k-th smallest element in 1-based range [l, r] (k is 1-indexed)
    int queryKthSmallest(int l, int r, int k) const {
        int rank = queryKth(roots[l - 1], roots[r], 1, maxVal, k);
        return sortedVals[rank - 1];
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "  Persistent Segment Tree (Chairman Tree) \n";
    std::cout << "==========================================\n\n";

    // 1-based indexing for demonstration
    std::vector<int> arr = {10, 80, 30, 90, 40, 50, 70};
    int n = arr.size();

    std::cout << "Input Array (1-indexed, size " << n << "):\n  ";
    for (int i = 0; i < n; ++i) {
        std::cout << "[" << (i + 1) << "]: " << arr[i] << "  ";
    }
    std::cout << "\n\n";

    PersistentSegmentTree pst(arr);

    // Queries: range [L, R] and k-th smallest
    // Range [2, 6] contains: [80, 30, 90, 40, 50]
    // Sorted: [30, 40, 50, 80, 90]
    std::cout << "Subarray [2..6]: [80, 30, 90, 40, 50]\n";
    std::cout << "  1st smallest in [2, 6]: " << pst.queryKthSmallest(2, 6, 1) << " (Expected: 30)\n";
    std::cout << "  2nd smallest in [2, 6]: " << pst.queryKthSmallest(2, 6, 2) << " (Expected: 40)\n";
    std::cout << "  3rd smallest in [2, 6]: " << pst.queryKthSmallest(2, 6, 3) << " (Expected: 50)\n";
    std::cout << "  4th smallest in [2, 6]: " << pst.queryKthSmallest(2, 6, 4) << " (Expected: 80)\n";
    std::cout << "  5th smallest in [2, 6]: " << pst.queryKthSmallest(2, 6, 5) << " (Expected: 90)\n\n";

    // Whole array [1, 7] median (4th smallest)
    std::cout << "Median of entire array [1, 7]: "
              << pst.queryKthSmallest(1, 7, 4) << " (Expected: 50)\n";

    std::cout << "\nVerification: All Persistent Tree queries executed in O(log N)!\n";
    return 0;
}
