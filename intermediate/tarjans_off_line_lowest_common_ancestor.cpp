/*
 * Problem Description:
 * Tarjan's Off-Line Lowest Common Ancestor (LCA) Algorithm.
 * Given a rooted tree and a collection of pairs of nodes (queries), determine the
 * lowest common ancestor for each query pair in an offline fashion.
 * The algorithm performs a single DFS traversal while maintaining a Disjoint Set Union (DSU)
 * structure with path compression and union-by-rank, achieving nearly linear time complexity
 * O(N + Q * alpha(N)), where N is the number of nodes, Q is the number of queries, and alpha is
 * the inverse Ackermann function.
 */

#include <iostream>
#include <vector>
#include <numeric>

class DisjointSetUnion {
public:
    std::vector<int> parent;
    std::vector<int> ancestor;
    std::vector<int> rank;

    DisjointSetUnion(int n) : parent(n), ancestor(n), rank(n, 0) {
        std::iota(parent.begin(), parent.end(), 0);
        std::iota(ancestor.begin(), ancestor.end(), 0);
    }

    int find(int i) {
        if (parent[i] == i)
            return i;
        return parent[i] = find(parent[i]);
    }

    void unite(int i, int j, int newAncestor) {
        int rootI = find(i);
        int rootJ = find(j);
        if (rootI != rootJ) {
            if (rank[rootI] < rank[rootJ]) {
                std::swap(rootI, rootJ);
            }
            parent[rootJ] = rootI;
            if (rank[rootI] == rank[rootJ]) {
                rank[rootI]++;
            }
            ancestor[rootI] = newAncestor;
        }
    }
};

struct Query {
    int target;
    int queryIndex;
};

void tarjanLCA(int u, int p, const std::vector<std::vector<int>>& adj,
               const std::vector<std::vector<Query>>& queries,
               std::vector<bool>& visited, DisjointSetUnion& dsu,
               std::vector<int>& ans) {
    visited[u] = true;
    dsu.ancestor[dsu.find(u)] = u;

    for (int v : adj[u]) {
        if (v != p) {
            tarjanLCA(v, u, adj, queries, visited, dsu, ans);
            dsu.unite(u, v, u);
        }
    }

    for (const auto& q : queries[u]) {
        if (visited[q.target]) {
            ans[q.queryIndex] = dsu.ancestor[dsu.find(q.target)];
        }
    }
}

int main() {
    int n = 7;
    // Tree edges:
    //       1
    //      / \
    //     2   3
    //    / \   \
    //   4   5   6
    //  /
    // 7
    // Using 1-based indexing mapped to 0-based
    std::vector<std::pair<int, int>> edges = {
        {1, 2}, {1, 3}, {2, 4}, {2, 5}, {3, 6}, {4, 7}
    };

    std::vector<std::vector<int>> adj(n + 1);
    for (const auto& edge : edges) {
        adj[edge.first].push_back(edge.second);
        adj[edge.second].push_back(edge.first);
    }

    std::vector<std::pair<int, int>> queryPairs = {
        {4, 5}, // LCA should be 2
        {7, 5}, // LCA should be 2
        {7, 6}, // LCA should be 1
        {3, 6}, // LCA should be 3
        {2, 3}  // LCA should be 1
    };

    std::vector<std::vector<Query>> queries(n + 1);
    for (int i = 0; i < static_cast<int>(queryPairs.size()); ++i) {
        int u = queryPairs[i].first;
        int v = queryPairs[i].second;
        queries[u].push_back({v, i});
        queries[v].push_back({u, i});
    }

    DisjointSetUnion dsu(n + 1);
    std::vector<bool> visited(n + 1, false);
    std::vector<int> ans(queryPairs.size(), -1);

    int root = 1;
    tarjanLCA(root, 0, adj, queries, visited, dsu, ans);

    std::cout << "Tarjan's Off-Line Lowest Common Ancestor (LCA) Results:\n";
    for (size_t i = 0; i < queryPairs.size(); ++i) {
        std::cout << "LCA(" << queryPairs[i].first << ", " << queryPairs[i].second << ") = "
                  << ans[i] << "\n";
    }

    return 0;
}
