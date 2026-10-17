/*
 * Program Description:
 * Heavy-Light Decomposition (HLD) on Trees in C++.
 * Decomposes an arbitrary tree of N nodes into a set of vertex-disjoint paths (heavy paths)
 * such that any simple path between two nodes u and v visits at most O(log N) distinct paths.
 *
 * Applications:
 * - Path queries (e.g. sum, max, min) on trees in O(log^2 N).
 * - Subtree queries in O(log N).
 * - Node updates in O(log N).
 *
 * Algorithm Steps:
 * 1. DFS 1 (size calculation): Computes subtree size, depth, parent, and identifies heavy edges.
 * 2. DFS 2 (decomposition): Assigns position in 1D array (pos[u]) and path head (head[u]).
 * 3. Segment Tree: Built over flattened 1D array representing concatenated heavy chains.
 *
 * Time Complexity:
 * - Preprocessing: O(N)
 * - Path Query: O(log^2 N)
 * - Node Update: O(log N)
 */

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

class SegmentTree {
    int n;
    std::vector<long long> tree;

    void update(int node, int start, int end, int idx, long long val) {
        if (start == end) {
            tree[node] = val;
            return;
        }
        int mid = start + (end - start) / 2;
        if (idx <= mid) update(2 * node, start, mid, idx, val);
        else update(2 * node + 1, mid + 1, end, idx, val);
        tree[node] = tree[2 * node] + tree[2 * node + 1];
    }

    long long query(int node, int start, int end, int l, int r) const {
        if (r < start || end < l) return 0;
        if (l <= start && end <= r) return tree[node];
        int mid = start + (end - start) / 2;
        return query(2 * node, start, mid, l, r) + query(2 * node + 1, mid + 1, end, l, r);
    }

public:
    explicit SegmentTree(int size) : n(size), tree(4 * size, 0) {}

    void update(int idx, long long val) {
        if (idx >= 0 && idx < n) update(1, 0, n - 1, idx, val);
    }

    long long query(int l, int r) const {
        if (l > r || l < 0 || r >= n) return 0;
        return query(1, 0, n - 1, l, r);
    }
};

class HeavyLightDecomposition {
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<int> parent, depth, heavy, head, pos;
    std::vector<long long> nodeValues;
    int curPos = 0;
    SegmentTree segTree;

    int dfsSize(int u, int p, int d) {
        parent[u] = p;
        depth[u] = d;
        int size = 1;
        int maxSubtree = 0;

        for (int v : adj[u]) {
            if (v != p) {
                int subSize = dfsSize(v, u, d + 1);
                size += subSize;
                if (subSize > maxSubtree) {
                    maxSubtree = subSize;
                    heavy[u] = v; // heavy child
                }
            }
        }
        return size;
    }

    void dfsDecompose(int u, int h) {
        head[u] = h;
        pos[u] = curPos++;
        segTree.update(pos[u], nodeValues[u]);

        // Prioritize heavy child to keep chain contiguous in segment tree
        if (heavy[u] != -1) {
            dfsDecompose(heavy[u], h);
        }

        // Decompose light children as roots of new chains
        for (int v : adj[u]) {
            if (v != parent[u] && v != heavy[u]) {
                dfsDecompose(v, v);
            }
        }
    }

public:
    HeavyLightDecomposition(int numNodes, const std::vector<long long>& values)
        : n(numNodes), adj(numNodes), parent(numNodes), depth(numNodes),
          heavy(numNodes, -1), head(numNodes), pos(numNodes),
          nodeValues(values), segTree(numNodes) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void init(int root = 0) {
        dfsSize(root, -1, 0);
        dfsDecompose(root, root);
    }

    void updateNode(int u, long long newVal) {
        nodeValues[u] = newVal;
        segTree.update(pos[u], newVal);
    }

    // Path sum query between u and v in O(log^2 N)
    long long queryPath(int u, int v) const {
        long long total = 0;
        while (head[u] != head[v]) {
            if (depth[head[u]] > depth[head[v]]) {
                std::swap(u, v);
            }
            // v's head is deeper, query from head[v] to v
            total += segTree.query(pos[head[v]], pos[v]);
            v = parent[head[v]];
        }
        // Both u and v are now on the same heavy path
        if (depth[u] > depth[v]) {
            std::swap(u, v);
        }
        total += segTree.query(pos[u], pos[v]);
        return total;
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "   Heavy-Light Decomposition (HLD) Demo   \n";
    std::cout << "==========================================\n\n";

    /*
     * Tree with 6 nodes (0 to 5):
     *        0 (val=1)
     *       / \
     *      1   2 (val=3)
     *     / \
     *    3   4 (val=5)
     *   /
     *  5 (val=6)
     */
    std::vector<long long> values = {1, 2, 3, 4, 5, 6};
    HeavyLightDecomposition hld(6, values);

    hld.addEdge(0, 1);
    hld.addEdge(0, 2);
    hld.addEdge(1, 3);
    hld.addEdge(1, 4);
    hld.addEdge(3, 5);

    hld.init(0);

    std::cout << "Tree node values: [0:1, 1:2, 2:3, 3:4, 4:5, 5:6]\n\n";

    // Query 1: Path 5 to 4: 5 -> 3 -> 1 -> 4 (sum: 6 + 4 + 2 + 5 = 17)
    std::cout << "Query Path (5 <-> 4) sum: " << hld.queryPath(5, 4) << " (Expected: 17)\n";

    // Query 2: Path 5 to 2: 5 -> 3 -> 1 -> 0 -> 2 (sum: 6 + 4 + 2 + 1 + 3 = 16)
    std::cout << "Query Path (5 <-> 2) sum: " << hld.queryPath(5, 2) << " (Expected: 16)\n";

    // Update node 1 from 2 to 10
    std::cout << "\nUpdating Node 1 value to 10...\n";
    hld.updateNode(1, 10);

    // Query 3: Path 5 to 4 sum should now be 17 + 8 = 25
    std::cout << "Query Path (5 <-> 4) sum: " << hld.queryPath(5, 4) << " (Expected: 25)\n";

    // Query 4: Path 5 to 2 sum should now be 16 + 8 = 24
    std::cout << "Query Path (5 <-> 2) sum: " << hld.queryPath(5, 2) << " (Expected: 24)\n";

    return 0;
}
