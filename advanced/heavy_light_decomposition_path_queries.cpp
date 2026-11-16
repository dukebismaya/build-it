/*
 * ============================================================================
 *  Euler Tour + Heavy Path Decomposition for Path Queries
 * ============================================================================
 *
 *  Heavy-Light Decomposition (HLD) decomposes a rooted tree into chains
 *  such that any root-to-leaf path crosses at most O(log N) chains. Combined
 *  with a segment tree on the linearized Euler tour, it answers path queries
 *  (sum, max, min) and path updates in O(log^2 N) time.
 *
 *  Algorithm:
 *  1) DFS to compute subtree sizes and identify heavy children
 *     (the child with the largest subtree).
 *  2) Second DFS assigns positions in the linearized array, keeping heavy
 *     paths contiguous.
 *  3) For a path query (u, v), climb the deeper node's chain head until both
 *     nodes share the same chain, querying the segment tree at each step.
 *
 *  This implementation supports:
 *  - Path sum query in O(log^2 N)
 *  - Path max query in O(log^2 N)
 *  - Point update in O(log N)
 *
 *  Time Complexity:
 *  - Preprocessing: O(N)
 *  - Path query/update: O(log^2 N)
 *  Space Complexity: O(N)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <climits>

namespace hld {

class SegTree {
public:
    SegTree(int n) : size(n), tree(4 * n, 0), maxTree(4 * n, INT64_MIN) {}

    void update(int pos, int64_t val, int node, int lo, int hi) {
        if (lo == hi) {
            tree[node] = val;
            maxTree[node] = val;
            return;
        }
        int mid = (lo + hi) / 2;
        if (pos <= mid) update(pos, val, 2 * node, lo, mid);
        else update(pos, val, 2 * node + 1, mid + 1, hi);
        tree[node] = tree[2 * node] + tree[2 * node + 1];
        maxTree[node] = std::max(maxTree[2 * node], maxTree[2 * node + 1]);
    }

    int64_t querySum(int l, int r, int node, int lo, int hi) const {
        if (r < lo || hi < l) return 0;
        if (l <= lo && hi <= r) return tree[node];
        int mid = (lo + hi) / 2;
        return querySum(l, r, 2 * node, lo, mid) +
               querySum(l, r, 2 * node + 1, mid + 1, hi);
    }

    int64_t queryMax(int l, int r, int node, int lo, int hi) const {
        if (r < lo || hi < l) return INT64_MIN;
        if (l <= lo && hi <= r) return maxTree[node];
        int mid = (lo + hi) / 2;
        return std::max(queryMax(l, r, 2 * node, lo, mid),
                        queryMax(l, r, 2 * node + 1, mid + 1, hi));
    }

    void update(int pos, int64_t val) { update(pos, val, 1, 0, size - 1); }
    int64_t querySum(int l, int r) const { return querySum(l, r, 1, 0, size - 1); }
    int64_t queryMax(int l, int r) const { return queryMax(l, r, 1, 0, size - 1); }

private:
    int size;
    std::vector<int64_t> tree, maxTree;
};

class HLD {
public:
    HLD(int n)
        : n(n), adj(n), weight(n, 0), parent(n, -1), depth(n, 0),
          heavy(n, -1), head(n, 0), pos(n, 0), subtreeSize(n, 0),
          seg(n), timer(0) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void setWeight(int node, int64_t w) { weight[node] = w; }

    void build(int root = 0) {
        dfs(root);
        decompose(root, root);
        /* Initialize segment tree with weights */
        for (int i = 0; i < n; ++i) {
            seg.update(pos[i], weight[i]);
        }
    }

    void updateNode(int v, int64_t newWeight) {
        weight[v] = newWeight;
        seg.update(pos[v], newWeight);
    }

    int64_t pathSum(int u, int v) const {
        int64_t result = 0;
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) std::swap(u, v);
            result += seg.querySum(pos[head[u]], pos[u]);
            u = parent[head[u]];
        }
        if (depth[u] > depth[v]) std::swap(u, v);
        result += seg.querySum(pos[u], pos[v]);
        return result;
    }

    int64_t pathMax(int u, int v) const {
        int64_t result = INT64_MIN;
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) std::swap(u, v);
            result = std::max(result, seg.queryMax(pos[head[u]], pos[u]));
            u = parent[head[u]];
        }
        if (depth[u] > depth[v]) std::swap(u, v);
        result = std::max(result, seg.queryMax(pos[u], pos[v]));
        return result;
    }

    int lca(int u, int v) const {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) std::swap(u, v);
            u = parent[head[u]];
        }
        return depth[u] < depth[v] ? u : v;
    }

private:
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<int64_t> weight;
    std::vector<int> parent, depth, heavy, head, pos, subtreeSize;
    SegTree seg;
    int timer;

    void dfs(int v, int par = -1, int dep = 0) {
        parent[v] = par;
        depth[v] = dep;
        subtreeSize[v] = 1;
        int maxChildSize = 0;

        for (int u : adj[v]) {
            if (u != par) {
                dfs(u, v, dep + 1);
                subtreeSize[v] += subtreeSize[u];
                if (subtreeSize[u] > maxChildSize) {
                    maxChildSize = subtreeSize[u];
                    heavy[v] = u;
                }
            }
        }
    }

    void decompose(int v, int h) {
        head[v] = h;
        pos[v] = timer++;

        if (heavy[v] != -1) {
            decompose(heavy[v], h); /* Continue heavy chain */
        }

        for (int u : adj[v]) {
            if (u != parent[v] && u != heavy[v]) {
                decompose(u, u); /* Start new chain */
            }
        }
    }
};

/* Brute force path sum/max for verification */
int64_t brutePathSum(const std::vector<std::vector<int>>& adj,
                     const std::vector<int64_t>& weight,
                     int u, int v, int n) {
    /* BFS to find path */
    std::vector<int> prev(n, -1);
    std::vector<bool> visited(n, false);
    std::vector<int> queue;
    queue.push_back(u);
    visited[u] = true;
    for (size_t qi = 0; qi < queue.size(); ++qi) {
        int cur = queue[qi];
        if (cur == v) break;
        for (int next : adj[cur]) {
            if (!visited[next]) {
                visited[next] = true;
                prev[next] = cur;
                queue.push_back(next);
            }
        }
    }

    int64_t sum = 0;
    int cur = v;
    while (cur != -1) {
        sum += weight[cur];
        if (cur == u) break;
        cur = prev[cur];
    }
    return sum;
}

} // namespace hld

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Heavy-Light Decomposition — Path Sum & Max Queries\n";
    std::cout << "========================================================\n\n";

    /*
     * Tree (rooted at 0):
     *        0(10)
     *       / \
     *      1(5) 2(8)
     *     /|     \
     *   3(3) 4(7) 5(2)
     *   /       / \
     *  6(1)   7(4) 8(6)
     */

    int n = 9;
    hld::HLD tree(n);

    tree.addEdge(0, 1); tree.addEdge(0, 2);
    tree.addEdge(1, 3); tree.addEdge(1, 4);
    tree.addEdge(2, 5); tree.addEdge(3, 6);
    tree.addEdge(5, 7); tree.addEdge(5, 8);

    int64_t weights[] = {10, 5, 8, 3, 7, 2, 1, 4, 6};
    for (int i = 0; i < n; ++i) tree.setWeight(i, weights[i]);

    tree.build(0);

    /* Path queries */
    struct PathQuery { int u, v; };
    std::vector<PathQuery> queries = {{6, 4}, {7, 8}, {6, 8}, {0, 7}, {3, 5}, {1, 2}};

    /* Build adjacency for brute force */
    std::vector<std::vector<int>> adj(n);
    adj[0] = {1, 2}; adj[1] = {0, 3, 4}; adj[2] = {0, 5};
    adj[3] = {1, 6}; adj[4] = {1}; adj[5] = {2, 7, 8};
    adj[6] = {3}; adj[7] = {5}; adj[8] = {5};
    std::vector<int64_t> wvec(weights, weights + n);

    std::cout << "Path Sum Queries:\n";
    std::cout << std::setw(12) << "Path" << std::setw(10) << "HLD"
              << std::setw(10) << "Brute" << std::setw(10) << "Match?" << "\n";
    std::cout << "------------------------------------------\n";

    bool allMatch = true;
    for (auto& q : queries) {
        int64_t hldResult = tree.pathSum(q.u, q.v);
        int64_t bruteResult = hld::brutePathSum(adj, wvec, q.u, q.v, n);
        bool match = (hldResult == bruteResult);
        if (!match) allMatch = false;

        std::string path = std::to_string(q.u) + " <-> " + std::to_string(q.v);
        std::cout << std::setw(12) << path << std::setw(10) << hldResult
                  << std::setw(10) << bruteResult
                  << std::setw(10) << (match ? "YES" : "NO") << "\n";
    }

    std::cout << "\nPath Max Queries:\n";
    std::cout << std::setw(12) << "Path" << std::setw(10) << "HLD Max" << "\n";
    std::cout << "----------------------\n";

    for (auto& q : queries) {
        int64_t maxVal = tree.pathMax(q.u, q.v);
        std::string path = std::to_string(q.u) + " <-> " + std::to_string(q.v);
        std::cout << std::setw(12) << path << std::setw(10) << maxVal << "\n";
    }

    /* LCA queries */
    std::cout << "\nLCA Queries:\n";
    for (auto& q : queries) {
        int lcaNode = tree.lca(q.u, q.v);
        std::cout << "  LCA(" << q.u << ", " << q.v << ") = " << lcaNode << "\n";
    }

    /* Update and re-query */
    std::cout << "\nAfter updating node 5: weight 2 -> 100\n";
    tree.updateNode(5, 100);
    std::cout << "  pathSum(7, 8) = " << tree.pathSum(7, 8) << "\n";
    std::cout << "  pathMax(0, 7) = " << tree.pathMax(0, 7) << "\n";

    std::cout << "\nAll sum queries verified: " << (allMatch ? "PASSED" : "FAILED") << "\n";

    return 0;
}
