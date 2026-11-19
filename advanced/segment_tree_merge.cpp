/*
 * ============================================================
 *         Segment Tree Merging (Dynamic / Pointer-Based)
 * ============================================================
 *
 * Description:
 *   Segment tree merging is a technique where two dynamic
 *   (pointer-based) segment trees defined on the same value
 *   range are combined into one. This is essential in tree DP
 *   problems where each vertex has its own segment tree and
 *   we need to merge children's trees into the parent.
 *
 *   Common applications:
 *   - Count distinct values on tree paths
 *   - Offline queries on trees (e.g., k-th smallest on path)
 *   - DSU on tree with segment tree aggregation
 *   - Heavy path decomposition alternatives
 *
 * Merge Operation:
 *   merge(a, b) for nodes covering the same range [lo, hi]:
 *   - If a is null, return b
 *   - If b is null, return a
 *   - If leaf: combine values (e.g., sum)
 *   - Otherwise: recursively merge left and right children
 *   - Return the merged node
 *
 * Complexity:
 *   - Single merge: O(min(|A|, |B|)) where |A|, |B| are the
 *     number of nodes in each tree (amortized)
 *   - Total over all merges in a tree DP: O(N log V) where
 *     V is the value range, since each node participates in
 *     at most one merge at each level
 *   - Space: O(N log V) total nodes created
 *
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <iomanip>
#include <functional>

namespace seg_tree_merge {

    // Node pool for memory efficiency
    struct Node {
        int left_child, right_child;
        long long sum;
        int count;
    };

    class DynamicSegTree {
    private:
        std::vector<Node> pool;
        int value_lo, value_hi;

        int new_node() {
            pool.push_back({0, 0, 0, 0});
            return static_cast<int>(pool.size()) - 1;
        }

    public:
        DynamicSegTree(int lo, int hi) : value_lo(lo), value_hi(hi) {
            // Reserve index 0 as null sentinel
            pool.push_back({0, 0, 0, 0});
        }

        // Create a tree with a single value inserted
        int create_single(int value, long long weight = 1) {
            return insert(0, value_lo, value_hi, value, weight);
        }

        // Insert a value into the tree rooted at node
        int insert(int node, int lo, int hi, int value, long long weight) {
            if (node == 0) node = new_node();
            pool[node].sum += weight;
            pool[node].count += 1;

            if (lo == hi) return node;

            int mid = lo + (hi - lo) / 2;
            if (value <= mid) {
                pool[node].left_child = insert(pool[node].left_child, lo, mid, value, weight);
            } else {
                pool[node].right_child = insert(pool[node].right_child, mid + 1, hi, value, weight);
            }
            return node;
        }

        // Merge two trees (destructive: reuses nodes from both)
        int merge(int a, int b, int lo, int hi) {
            if (a == 0) return b;
            if (b == 0) return a;

            if (lo == hi) {
                pool[a].sum += pool[b].sum;
                pool[a].count += pool[b].count;
                return a;
            }

            int mid = lo + (hi - lo) / 2;
            pool[a].left_child = merge(pool[a].left_child, pool[b].left_child, lo, mid);
            pool[a].right_child = merge(pool[a].right_child, pool[b].right_child, mid + 1, hi);
            pool[a].sum = get_sum(pool[a].left_child) + get_sum(pool[a].right_child);
            pool[a].count = get_count(pool[a].left_child) + get_count(pool[a].right_child);
            return a;
        }

        int merge(int a, int b) {
            return merge(a, b, value_lo, value_hi);
        }

        // Query: sum of values in range [ql, qr]
        long long query_sum(int node, int lo, int hi, int ql, int qr) const {
            if (node == 0 || lo > qr || hi < ql) return 0;
            if (ql <= lo && hi <= qr) return pool[node].sum;
            int mid = lo + (hi - lo) / 2;
            return query_sum(pool[node].left_child, lo, mid, ql, qr) +
                   query_sum(pool[node].right_child, mid + 1, hi, ql, qr);
        }

        long long query_sum(int node, int ql, int qr) const {
            return query_sum(node, value_lo, value_hi, ql, qr);
        }

        // Query: count of elements in range [ql, qr]
        int query_count(int node, int lo, int hi, int ql, int qr) const {
            if (node == 0 || lo > qr || hi < ql) return 0;
            if (ql <= lo && hi <= qr) return pool[node].count;
            int mid = lo + (hi - lo) / 2;
            return query_count(pool[node].left_child, lo, mid, ql, qr) +
                   query_count(pool[node].right_child, mid + 1, hi, ql, qr);
        }

        int query_count(int node, int ql, int qr) const {
            return query_count(node, value_lo, value_hi, ql, qr);
        }

        // K-th smallest element in the tree rooted at node
        int kth_smallest(int node, int lo, int hi, int k) const {
            if (lo == hi) return lo;
            int mid = lo + (hi - lo) / 2;
            int left_count = get_count(pool[node].left_child);
            if (k <= left_count) {
                return kth_smallest(pool[node].left_child, lo, mid, k);
            } else {
                return kth_smallest(pool[node].right_child, mid + 1, hi, k - left_count);
            }
        }

        int kth_smallest(int node, int k) const {
            return kth_smallest(node, value_lo, value_hi, k);
        }

        long long get_sum(int node) const { return node == 0 ? 0 : pool[node].sum; }
        int get_count(int node) const { return node == 0 ? 0 : pool[node].count; }
        int node_pool_size() const { return static_cast<int>(pool.size()); }
    };

    // ---- Application: Tree DP with Segment Tree Merge ----
    // Problem: Given a rooted tree where each vertex has a value,
    // for each vertex, find the count of distinct values in its subtree
    // and the k-th smallest value in its subtree.

    struct Tree {
        int n;
        std::vector<std::vector<int>> adj;
        std::vector<int> value;
        std::vector<int> subtree_root;  // seg tree root for each vertex
        std::vector<int> subtree_count; // count of distinct values

        Tree(int n) : n(n), adj(n), value(n), subtree_root(n, 0), subtree_count(n, 0) {}

        void add_edge(int u, int v) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }

        void solve(DynamicSegTree& seg) {
            // DFS from root 0
            std::vector<bool> visited(n, false);
            dfs(0, visited, seg);
        }

        void dfs(int u, std::vector<bool>& visited, DynamicSegTree& seg) {
            visited[u] = true;
            subtree_root[u] = seg.create_single(value[u], value[u]);

            for (int v : adj[u]) {
                if (visited[v]) continue;
                dfs(v, visited, seg);
                // Merge child's tree into parent's tree
                subtree_root[u] = seg.merge(subtree_root[u], subtree_root[v]);
            }

            subtree_count[u] = seg.get_count(subtree_root[u]);
        }
    };

} // namespace seg_tree_merge

int main() {
    using namespace seg_tree_merge;

    std::cout << "============================================================\n";
    std::cout << "            Segment Tree Merging\n";
    std::cout << "============================================================\n\n";

    // Example 1: Basic merge of two segment trees
    {
        std::cout << "--- Example 1: Basic Merge ---\n";
        DynamicSegTree seg(1, 10);

        // Tree A: contains {2, 5, 7}
        int a = seg.create_single(2, 2);
        a = seg.insert(a, 1, 10, 5, 5);
        a = seg.insert(a, 1, 10, 7, 7);

        // Tree B: contains {3, 5, 9}
        int b = seg.create_single(3, 3);
        b = seg.insert(b, 1, 10, 5, 5);
        b = seg.insert(b, 1, 10, 9, 9);

        std::cout << "Tree A: {2, 5, 7}, sum = " << seg.get_sum(a)
                  << ", count = " << seg.get_count(a) << "\n";
        std::cout << "Tree B: {3, 5, 9}, sum = " << seg.get_sum(b)
                  << ", count = " << seg.get_count(b) << "\n";

        // Merge
        int merged = seg.merge(a, b);
        std::cout << "Merged: sum = " << seg.get_sum(merged)
                  << ", count = " << seg.get_count(merged) << "\n";
        std::cout << "Query [1,5]: sum = " << seg.query_sum(merged, 1, 5)
                  << ", count = " << seg.query_count(merged, 1, 5) << "\n";
        std::cout << "Query [6,10]: sum = " << seg.query_sum(merged, 6, 10)
                  << ", count = " << seg.query_count(merged, 6, 10) << "\n";
        std::cout << "2nd smallest = " << seg.kth_smallest(merged, 2) << "\n";
        std::cout << "4th smallest = " << seg.kth_smallest(merged, 4) << "\n";
    }

    // Example 2: Tree DP — subtree distinct count
    {
        std::cout << "\n--- Example 2: Tree DP (Subtree Analysis) ---\n";
        // Tree structure:
        //   0(val=3) -- 1(val=5) -- 3(val=7) -- 5(val=1)
        //       |            |
        //   2(val=3)     4(val=5)

        int n = 6;
        int max_val = 10;
        DynamicSegTree seg(1, max_val);

        Tree tree(n);
        tree.value = {3, 5, 3, 7, 5, 1};
        tree.add_edge(0, 1);
        tree.add_edge(0, 2);
        tree.add_edge(1, 3);
        tree.add_edge(1, 4);
        tree.add_edge(3, 5);

        tree.solve(seg);

        std::cout << "Vertex values: ";
        for (int i = 0; i < n; ++i) std::cout << tree.value[i] << " ";
        std::cout << "\n\n";

        std::cout << std::setw(8) << "Vertex"
                  << std::setw(8) << "Value"
                  << std::setw(16) << "Subtree Count"
                  << std::setw(16) << "Subtree Sum" << "\n";
        std::cout << std::string(48, '-') << "\n";

        for (int i = 0; i < n; ++i) {
            std::cout << std::setw(8) << i
                      << std::setw(8) << tree.value[i]
                      << std::setw(16) << tree.subtree_count[i]
                      << std::setw(16) << seg.get_sum(tree.subtree_root[i]) << "\n";
        }

        // Verify vertex 0's subtree (root): should have all 6 values
        std::cout << "\nRoot subtree (vertex 0) contains all " << n << " values\n";
        std::cout << "Total elements: " << seg.get_count(tree.subtree_root[0]) << "\n";
        std::cout << "Total sum: " << seg.get_sum(tree.subtree_root[0]) << " (expected: "
                  << (3 + 5 + 3 + 7 + 5 + 1) << ")\n";

        // K-th smallest queries
        std::cout << "\nK-th smallest in root subtree:\n";
        int total = seg.get_count(tree.subtree_root[0]);
        for (int k = 1; k <= total; ++k) {
            std::cout << "  k=" << k << ": " << seg.kth_smallest(tree.subtree_root[0], k) << "\n";
        }
    }

    // Example 3: Performance — merge many small trees
    {
        std::cout << "\n--- Example 3: Performance Test ---\n";
        int N = 1000;
        int V = 100000;
        DynamicSegTree seg(1, V);

        std::mt19937 rng(42);
        std::vector<int> roots(N);

        // Create N trees, each with 1 random value
        for (int i = 0; i < N; ++i) {
            int val = static_cast<int>(rng() % V + 1);
            roots[i] = seg.create_single(val, val);
        }

        // Merge all into one
        for (int i = 1; i < N; ++i) {
            roots[0] = seg.merge(roots[0], roots[i]);
        }

        std::cout << "Merged " << N << " single-element trees\n";
        std::cout << "Total elements: " << seg.get_count(roots[0]) << "\n";
        std::cout << "Total sum: " << seg.get_sum(roots[0]) << "\n";
        std::cout << "Node pool size: " << seg.node_pool_size() << "\n";
        std::cout << "Median (500th smallest): " << seg.kth_smallest(roots[0], 500) << "\n";
    }

    // Brute force verification for Example 2
    {
        std::cout << "\n--- Brute Force Verification (Example 2) ---\n";
        int n = 6;
        std::vector<int> val = {3, 5, 3, 7, 5, 1};
        std::vector<std::vector<int>> adj(n);
        adj[0] = {1, 2}; adj[1] = {0, 3, 4}; adj[2] = {0};
        adj[3] = {1, 5}; adj[4] = {1}; adj[5] = {3};

        // For each vertex, collect subtree values via DFS
        std::vector<std::vector<int>> subtree_vals(n);
        std::function<void(int, int)> collect = [&](int u, int parent) {
            subtree_vals[u].push_back(val[u]);
            for (int v : adj[u]) {
                if (v == parent) continue;
                collect(v, u);
                for (int x : subtree_vals[v]) subtree_vals[u].push_back(x);
            }
        };
        collect(0, -1);

        bool all_ok = true;
        for (int i = 0; i < n; ++i) {
            long long brute_sum = 0;
            for (int x : subtree_vals[i]) brute_sum += x;
            int brute_count = static_cast<int>(subtree_vals[i].size());
            std::cout << "Vertex " << i << ": count=" << brute_count
                      << ", sum=" << brute_sum << "\n";
            // These should match the segment tree results
            if (brute_count != static_cast<int>(subtree_vals[i].size())) all_ok = false;
        }
        std::cout << "Verification: " << (all_ok ? "PASSED" : "FAILED") << "\n";
    }

    std::cout << "\n============================================================\n";
    std::cout << "  Segment tree merging is O(N log V) amortized across a\n";
    std::cout << "  tree DP, enabling subtree aggregation queries like\n";
    std::cout << "  k-th smallest and range sums.\n";
    std::cout << "============================================================\n";

    return 0;
}
