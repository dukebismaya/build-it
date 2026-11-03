/**
 * @file minimum_weight_vertex_cover_tree.cpp
 * @brief Computes the Minimum Weight Vertex Cover on a tree in O(V) time using Tree DP.
 *
 * Problem:
 * Given a tree T = (V, E) where each vertex v in V has a non-negative weight w(v),
 * find a subset of vertices S subseteq V of minimum total weight such that every edge
 * (u, v) in E has at least one endpoint in S (u in S or v in S).
 *
 * Tree Dynamic Programming Recurrence:
 * Root the tree arbitrarily at node 0.
 * For each node u:
 *   dp[u][0]: Minimum weight of vertex cover in subtree(u) where u is NOT chosen.
 *             All children v of u MUST be chosen to cover edges (u, v):
 *             dp[u][0] = sum_{v in children(u)} dp[v][1]
 *
 *   dp[u][1]: Minimum weight of vertex cover in subtree(u) where u IS chosen.
 *             Each child v can be either chosen or not:
 *             dp[u][1] = weight[u] + sum_{v in children(u)} min(dp[v][0], dp[v][1])
 *
 * Minimum total weight = min(dp[root][0], dp[root][1]).
 * The exact subset S is reconstructed via DFS.
 *
 * Time Complexity: O(V)
 * Space Complexity: O(V)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>

struct TreeVertexCoverResult {
    long long minWeight;
    std::vector<int> chosenVertices;
};

class TreeVertexCoverSolver {
private:
    int n;
    std::vector<long long> weights;
    std::vector<std::vector<int>> adj;
    std::vector<std::vector<long long>> dp;

    void dfs(int u, int p) {
        dp[u][0] = 0;
        dp[u][1] = weights[u];

        for (int v : adj[u]) {
            if (v == p) continue;
            dfs(v, u);

            dp[u][0] += dp[v][1];
            dp[u][1] += std::min(dp[v][0], dp[v][1]);
        }
    }

    void reconstruct(int u, int p, bool isParentIncluded, std::vector<int>& chosen) {
        bool includeCurrent = false;

        if (!isParentIncluded) {
            // Must include current node
            includeCurrent = true;
        } else {
            // Can choose cheaper option
            if (dp[u][1] < dp[u][0]) {
                includeCurrent = true;
            } else {
                includeCurrent = false;
            }
        }

        if (includeCurrent) {
            chosen.push_back(u);
        }

        for (int v : adj[u]) {
            if (v == p) continue;
            reconstruct(v, u, includeCurrent, chosen);
        }
    }

public:
    TreeVertexCoverSolver(int vertices, std::vector<long long> vertexWeights)
        : n(vertices), weights(std::move(vertexWeights)), adj(vertices), dp(vertices, std::vector<long long>(2, 0)) {}

    void addEdge(int u, int v) {
        if (u >= 0 && u < n && v >= 0 && v < n) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }

    TreeVertexCoverResult solve() {
        if (n == 0) return {0, {}};
        if (n == 1) return {0, {}}; // An edge-less graph requires 0 vertices for vertex cover

        dfs(0, -1);

        long long bestWeight = std::min(dp[0][0], dp[0][1]);
        std::vector<int> chosen;

        bool rootIncluded = (dp[0][1] < dp[0][0]);
        if (rootIncluded) {
            chosen.push_back(0);
        }

        for (int v : adj[0]) {
            reconstruct(v, 0, rootIncluded, chosen);
        }

        std::sort(chosen.begin(), chosen.end());
        return {bestWeight, chosen};
    }
};

int main() {
    std::cout << "=== Minimum Weight Vertex Cover on Trees (Tree DP) ===\n\n";

    // Tree Structure (7 nodes, 0-indexed):
    //         0 (wt 10)
    //        / \
    //   1(wt 5) 2(wt 3)
    //    /   \     \
    // 3(1)   4(2)  5(4)
    //                \
    //                6(1)
    std::vector<long long> weights = {10, 5, 3, 1, 2, 4, 1};
    int n = static_cast<int>(weights.size());

    TreeVertexCoverSolver solver(n, weights);
    solver.addEdge(0, 1);
    solver.addEdge(0, 2);
    solver.addEdge(1, 3);
    solver.addEdge(1, 4);
    solver.addEdge(2, 5);
    solver.addEdge(5, 6);

    std::cout << "Tree Node Weights:\n";
    for (int i = 0; i < n; ++i) {
        std::cout << "  Node " << i << ": Weight = " << weights[i] << '\n';
    }

    auto res = solver.solve();

    std::cout << "\nOptimal Vertex Cover Result:\n";
    std::cout << "  Minimum Total Weight: " << res.minWeight << '\n';
    std::cout << "  Vertices in Cover S:  { ";
    for (int u : res.chosenVertices) {
        std::cout << u << " (wt " << weights[u] << ") ";
    }
    std::cout << "}\n";

    return 0;
}
