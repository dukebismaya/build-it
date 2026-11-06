/**
 * @file gomory_hu_cut_tree.cpp
 * @brief Computes the Gomory-Hu Cut Tree representing all V*(V-1)/2 all-pairs min-cuts using V-1 max flows.
 *
 * Theory:
 * Published by Ralph Gomory and T.C. Hu in 1961.
 * For an undirected weighted graph G = (V, E), the Gomory-Hu tree T is a weighted tree
 * on the same vertex set V such that for ANY pair of vertices (u, v):
 *   1. The minimum capacity of an edge on the path between u and v in T equals the
 *      minimum cut capacity between u and v in G.
 *   2. Removing that minimal edge in T produces the exact two disconnected components
 *      of the minimum cut in G!
 *
 * Key Significance:
 * Represents all O(V^2) pairwise minimum cuts using only V - 1 max-flow computations,
 * instead of running O(V^2) separate flow algorithms!
 *
 * Time Complexity: O(V * Dinic(V, E)) ~ O(V^2 * E)
 * Space Complexity: O(V + E)
 */

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <iomanip>

struct FlowEdge {
    int to;
    int rev;
    long long cap;
    long long flow;
};

class DinicFlow {
public:
    static constexpr long long INF = std::numeric_limits<long long>::max() / 4;

    int n;
    std::vector<std::vector<FlowEdge>> adj;
    std::vector<int> level;
    std::vector<int> ptr;

    explicit DinicFlow(int vertices) : n(vertices), adj(vertices), level(vertices), ptr(vertices) {}

    void addEdge(int from, int to, long long cap) {
        adj[from].push_back({to, static_cast<int>(adj[to].size()), cap, 0});
        adj[to].push_back({from, static_cast<int>(adj[from].size()) - 1, cap, 0}); // undirected edge
    }

    bool bfs(int s, int t) {
        std::fill(level.begin(), level.end(), -1);
        level[s] = 0;
        std::queue<int> q;
        q.push(s);

        while (!q.empty()) {
            int v = q.front();
            q.pop();

            for (const auto& edge : adj[v]) {
                if (edge.cap - edge.flow > 0 && level[edge.to] == -1) {
                    level[edge.to] = level[v] + 1;
                    q.push(edge.to);
                }
            }
        }
        return level[t] != -1;
    }

    long long dfs(int v, int t, long long pushed) {
        if (pushed == 0 || v == t) return pushed;

        for (int& cid = ptr[v]; cid < static_cast<int>(adj[v].size()); ++cid) {
            auto& edge = adj[v][cid];
            int trg = edge.to;

            if (level[v] + 1 != level[trg] || edge.cap - edge.flow <= 0) continue;

            long long tr = dfs(trg, t, std::min(pushed, edge.cap - edge.flow));
            if (tr == 0) continue;

            edge.flow += tr;
            adj[trg][edge.rev].flow -= tr;
            return tr;
        }
        return 0;
    }

    long long maxFlow(int s, int t) {
        for (auto& row : adj) {
            for (auto& edge : row) edge.flow = 0;
        }

        long long flow = 0;
        while (bfs(s, t)) {
            std::fill(ptr.begin(), ptr.end(), 0);
            while (long long pushed = dfs(s, t, INF)) {
                flow += pushed;
            }
        }
        return flow;
    }

    std::vector<bool> getCutReachable(int s) {
        std::vector<bool> vis(n, false);
        std::queue<int> q;
        q.push(s);
        vis[s] = true;

        while (!q.empty()) {
            int v = q.front();
            q.pop();

            for (const auto& edge : adj[v]) {
                if (edge.cap - edge.flow > 0 && !vis[edge.to]) {
                    vis[edge.to] = true;
                    q.push(edge.to);
                }
            }
        }
        return vis;
    }
};

struct TreeEdge {
    int u;
    int v;
    long long weight;
};

class GomoryHuTree {
public:
    static std::vector<TreeEdge> build(int n, const std::vector<std::pair<std::pair<int, int>, long long>>& edges) {
        std::vector<int> parent(n, 0);
        std::vector<long long> weight(n, 0);

        for (int i = 1; i < n; ++i) {
            DinicFlow flow(n);
            for (const auto& e : edges) {
                flow.addEdge(e.first.first, e.first.second, e.second);
            }

            int p = parent[i];
            long long cutVal = flow.maxFlow(i, p);
            weight[i] = cutVal;

            auto inCut = flow.getCutReachable(i);

            for (int j = i + 1; j < n; ++j) {
                if (parent[j] == p && inCut[j]) {
                    parent[j] = i;
                }
            }
        }

        std::vector<TreeEdge> cutTree;
        for (int i = 1; i < n; ++i) {
            cutTree.push_back({i, parent[i], weight[i]});
        }
        return cutTree;
    }
};

int main() {
    std::cout << "=== Gomory-Hu Cut Tree (All-Pairs Min-Cut via V-1 Flows) ===\n\n";

    // Graph with 5 vertices (0 to 4):
    int n = 5;
    std::vector<std::pair<std::pair<int, int>, long long>> edges = {
        {{0, 1}, 2},
        {{0, 2}, 3},
        {{1, 2}, 1},
        {{1, 3}, 1},
        {{1, 4}, 1},
        {{2, 4}, 2},
        {{3, 4}, 3}
    };

    std::cout << "Input Undirected Graph (5 Vertices, 7 Weighted Edges):\n";
    for (const auto& e : edges) {
        std::cout << "  Edge (" << e.first.first << " - " << e.first.second 
                  << ") : Capacity = " << e.second << '\n';
    }

    auto tree = GomoryHuTree::build(n, edges);

    std::cout << "\nComputed Gomory-Hu Tree (4 Tree Edges):\n";
    std::cout << "  " << std::setw(12) << "Tree Edge"
              << std::setw(16) << "Min Cut Capacity" << '\n';
    std::cout << "  " << std::string(30, '-') << '\n';

    for (const auto& e : tree) {
        std::string edgeStr = "(" + std::to_string(e.u) + " - " + std::to_string(e.v) + ")";
        std::cout << "  " << std::setw(12) << edgeStr
                  << std::setw(16) << e.weight << '\n';
    }

    std::cout << "\nAny (u, v) min-cut is now found in O(V) via path bottleneck on this tree!\n";

    return 0;
}

