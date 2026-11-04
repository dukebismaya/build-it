/**
 * @file tarjans_offline_lca.cpp
 * @brief Computes Lowest Common Ancestors (LCA) for multiple queries offline using Tarjan's DSU Algorithm.
 *
 * Theory:
 * Invented by Robert Tarjan in 1979, this algorithm answers Q lowest common ancestor
 * queries on a tree with V vertices in O(V + Q * alpha(V)) time.
 *
 * Concepts:
 * - Offline: all queries are known in advance.
 * - Colors:
 *   - WHITE: Node not yet visited.
 *   - GRAY:  Node currently on DFS recursion stack (active ancestor).
 *   - BLACK: Node and all its subtrees have finished processing.
 *
 * Algorithm Steps:
 * 1. Root the tree arbitrarily at node 0.
 * 2. Maintain a Disjoint Set Union (DSU) where each set stores its highest 'ancestor' pointer.
 * 3. During DFS traversal at node u:
 *    - Set ancestor[u] = u.
 *    - For each child v of u:
 *        dfs(v)
 *        unionSets(u, v)
 *        ancestor[find(u)] = u
 *    - Mark u as BLACK.
 *    - For each query {u, v}:
 *        If v is already marked BLACK:
 *          LCA(u, v) = ancestor[find(v)]
 *
 * Time Complexity: O(V + Q * alpha(V)) where alpha is the inverse Ackermann function
 * Space Complexity: O(V + Q)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

struct Query {
    int u;
    int v;
    int id;
};

class TarjanOfflineLCA {
private:
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<std::vector<std::pair<int, int>>> queryList; // queryList[u] = {v, queryId}
    std::vector<int> parent;
    std::vector<int> ancestor;
    std::vector<bool> visited; // true when node turns BLACK
    std::vector<int> lcaResult;

    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]);
    }

    void unionSets(int i, int j) {
        int rootI = find(i);
        int rootJ = find(j);
        if (rootI != rootJ) {
            parent[rootI] = rootJ;
        }
    }

    void dfs(int u, int p) {
        ancestor[u] = u;

        for (int v : adj[u]) {
            if (v == p) continue;
            dfs(v, u);
            unionSets(v, u);
            ancestor[find(u)] = u;
        }

        visited[u] = true;

        for (const auto& [v, qId] : queryList[u]) {
            if (visited[v]) {
                lcaResult[qId] = ancestor[find(v)];
            }
        }
    }

public:
    explicit TarjanOfflineLCA(int vertices)
        : n(vertices),
          adj(vertices),
          queryList(vertices),
          parent(vertices),
          ancestor(vertices),
          visited(vertices, false) {
        for (int i = 0; i < n; ++i) {
            parent[i] = i;
            ancestor[i] = i;
        }
    }

    void addEdge(int u, int v) {
        if (u >= 0 && u < n && v >= 0 && v < n) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }

    void addQuery(int u, int v, int qId) {
        queryList[u].push_back({v, qId});
        queryList[v].push_back({u, qId});
    }

    std::vector<int> solveQueries(int root, int numQueries) {
        lcaResult.assign(numQueries, -1);
        dfs(root, -1);
        return lcaResult;
    }
};

int main() {
    std::cout << "=== Tarjan's Offline LCA Algorithm (O(V + Q * alpha(V))) ===\n\n";

    /*
     * Tree Structure (9 vertices, root = 0):
     *            0
     *          /   \
     *         1     2
     *        / \   / \
     *       3   4 5   6
     *          /       \
     *         7         8
     */
    int n = 9;
    TarjanOfflineLCA solver(n);
    solver.addEdge(0, 1);
    solver.addEdge(0, 2);
    solver.addEdge(1, 3);
    solver.addEdge(1, 4);
    solver.addEdge(2, 5);
    solver.addEdge(2, 6);
    solver.addEdge(4, 7);
    solver.addEdge(6, 8);

    std::vector<std::pair<int, int>> queries = {
        {3, 7}, // LCA(3, 7) = 1
        {7, 8}, // LCA(7, 8) = 0
        {5, 8}, // LCA(5, 8) = 2
        {1, 7}, // LCA(1, 7) = 1
        {3, 4}, // LCA(3, 4) = 1
        {0, 8}  // LCA(0, 8) = 0
    };

    for (size_t i = 0; i < queries.size(); ++i) {
        solver.addQuery(queries[i].first, queries[i].second, static_cast<int>(i));
    }

    auto answers = solver.solveQueries(0, static_cast<int>(queries.size()));

    std::cout << "Tree LCA Query Results:\n";
    std::cout << "  " << std::setw(12) << "Query"
              << std::setw(12) << "Node U"
              << std::setw(12) << "Node V"
              << std::setw(14) << "Computed LCA" << '\n';
    std::cout << "  " << std::string(50, '-') << '\n';

    for (size_t i = 0; i < queries.size(); ++i) {
        std::cout << "  " << std::setw(12) << ("Q" + std::to_string(i + 1))
                  << std::setw(12) << queries[i].first
                  << std::setw(12) << queries[i].second
                  << std::setw(14) << answers[i] << '\n';
    }

    return 0;
}
