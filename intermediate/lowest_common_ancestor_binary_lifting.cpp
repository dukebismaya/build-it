/*
 * Program Description:
 * Lowest Common Ancestor (LCA) using Binary Lifting on Trees.
 *
 * Algorithm Details:
 * 1. Precomputes up[u][i] = 2^i-th ancestor of node u using DFS.
 * 2. Precomputation Time: O(N log N), Space: O(N log N).
 * 3. LCA Query:
 *    - Lift the deeper node to the same depth as the other node in O(log N).
 *    - If they are already the same, that node is the LCA.
 *    - Otherwise, simultaneously lift both nodes upwards until they are just below their LCA.
 *    - Query Time: O(log N).
 * 4. Tree distance query: dist(u, v) = depth[u] + depth[v] - 2 * depth[LCA(u, v)].
 */

#include <iostream>
#include <vector>
#include <cmath>

class TreeLCA {
    int n;
    int maxLog;
    std::vector<std::vector<int>> adj;
    std::vector<std::vector<int>> up; // up[node][i] = 2^i-th ancestor of node
    std::vector<int> depth;

    void dfs(int u, int p, int d) {
        depth[u] = d;
        up[u][0] = p;

        for (int i = 1; i < maxLog; ++i) {
            if (up[u][i - 1] != -1) {
                up[u][i] = up[up[u][i - 1]][i - 1];
            } else {
                up[u][i] = -1;
            }
        }

        for (int v : adj[u]) {
            if (v != p) {
                dfs(v, u, d + 1);
            }
        }
    }

public:
    explicit TreeLCA(int nodes) : n(nodes), adj(nodes), depth(nodes, 0) {
        maxLog = static_cast<int>(std::ceil(std::log2(n + 1))) + 1;
        up.assign(n, std::vector<int>(maxLog, -1));
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void preprocess(int root = 0) {
        dfs(root, -1, 0);
    }

    int getLCA(int u, int v) const {
        if (depth[u] < depth[v]) {
            std::swap(u, v);
        }

        // Lift u up to the same depth as v
        int diff = depth[u] - depth[v];
        for (int i = 0; i < maxLog; ++i) {
            if ((diff >> i) & 1) {
                u = up[u][i];
            }
        }

        if (u == v) return u;

        // Lift both simultaneously
        for (int i = maxLog - 1; i >= 0; --i) {
            if (up[u][i] != up[v][i]) {
                u = up[u][i];
                v = up[v][i];
            }
        }

        return up[u][0];
    }

    int getDistance(int u, int v) const {
        int lca = getLCA(u, v);
        return depth[u] + depth[v] - 2 * depth[lca];
    }

    int getDepth(int u) const {
        return depth[u];
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "  Lowest Common Ancestor (Binary Lifting) \n";
    std::cout << "==========================================\n\n";

    /*
     * Build sample tree with 9 nodes (0-indexed):
     *        0
     *       / \
     *      1   2
     *     / \   \
     *    3   4   5
     *   / \   \
     *  6   7   8
     */
    TreeLCA tree(9);
    tree.addEdge(0, 1);
    tree.addEdge(0, 2);
    tree.addEdge(1, 3);
    tree.addEdge(1, 4);
    tree.addEdge(2, 5);
    tree.addEdge(3, 6);
    tree.addEdge(3, 7);
    tree.addEdge(4, 8);

    tree.preprocess(0);

    std::vector<std::pair<int, int>> queries = {
        {6, 7}, // LCA: 3
        {6, 8}, // LCA: 1
        {7, 5}, // LCA: 0
        {3, 4}, // LCA: 1
        {8, 1}, // LCA: 1
        {5, 2}  // LCA: 2
    };

    std::cout << "--- LCA & Distance Queries ---\n";
    for (const auto& [u, v] : queries) {
        int lca = tree.getLCA(u, v);
        int dist = tree.getDistance(u, v);
        std::cout << "Nodes (" << u << ", " << v << "): "
                  << "LCA = " << lca << ", Tree Distance = " << dist << " edges\n";
    }

    return 0;
}
