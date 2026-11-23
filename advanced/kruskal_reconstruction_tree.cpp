/*
 * ============================================================
 *               Kruskal Reconstruction Tree (KRT)
 * ============================================================
 *
 * Description:
 *   While running Kruskal's algorithm, every time an edge (u,v,w)
 *   joins two components, create a NEW node with value w whose
 *   two children are the roots of those components. The result
 *   is a binary tree with N leaves (original vertices) and N-1
 *   internal nodes (MST edges), where values are non-decreasing
 *   toward the root.
 *
 * Key properties:
 *   - The minimax path weight between u and v (the smallest
 *     possible maximum edge on any u-v path) equals the value
 *     of LCA(u, v) in the KRT.
 *   - The set of vertices reachable from v using only edges of
 *     weight <= W is exactly the leaf set of the highest
 *     ancestor of v whose value is <= W (found by binary lifting).
 *
 * Complexity:
 *   Build: O(M log M) (sort) + O(N log N) (lifting tables)
 *   Query (minimax / reachable count): O(log N)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <queue>
#include <tuple>

namespace krt {

    struct Edge { int u, v, w; };

    class KruskalTree {
        int n, LOG, nodes;
        std::vector<int> dsu, val, leaves, depth;
        std::vector<std::vector<int>> up, children;

        int find(int x) { return dsu[x] == x ? x : dsu[x] = find(dsu[x]); }

    public:
        KruskalTree(int n, std::vector<Edge> edges) : n(n) {
            int total = 2 * n - 1;
            LOG = 1; while ((1 << LOG) < total) ++LOG;
            dsu.resize(total); std::iota(dsu.begin(), dsu.end(), 0);
            val.assign(total, 0); leaves.assign(total, 0); depth.assign(total, 0);
            children.assign(total, {});
            up.assign(LOG + 1, std::vector<int>(total, -1));
            for (int i = 0; i < n; ++i) leaves[i] = 1;

            std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) { return a.w < b.w; });
            nodes = n;
            for (auto& e : edges) {
                int a = find(e.u), b = find(e.v);
                if (a == b) continue;
                int c = nodes++;
                val[c] = e.w;
                dsu[a] = dsu[b] = c;
                up[0][a] = up[0][b] = c;
                children[c] = {a, b};
                leaves[c] = leaves[a] + leaves[b];
            }
            // depths (top-down from every root), then lifting tables
            for (int v = nodes - 1; v >= 0; --v)
                if (up[0][v] != -1) depth[v] = depth[up[0][v]] + 1;
            for (int k = 1; k <= LOG; ++k)
                for (int v = 0; v < nodes; ++v)
                    up[k][v] = up[k - 1][v] == -1 ? -1 : up[k - 1][up[k - 1][v]];
        }

        // Minimax edge weight on a path u-v; -1 if disconnected
        int minimax(int u, int v) {
            if (u == v) return 0;
            if (find(u) != find(v)) return -1;
            if (depth[u] < depth[v]) std::swap(u, v);
            for (int k = LOG; k >= 0; --k)
                if (up[k][u] != -1 && depth[up[k][u]] >= depth[v]) u = up[k][u];
            if (u == v) return val[u];
            for (int k = LOG; k >= 0; --k)
                if (up[k][u] != up[k][v]) { u = up[k][u]; v = up[k][v]; }
            return val[up[0][u]];
        }

        // Number of vertices reachable from v using edges with weight <= W
        int reachable(int v, int W) {
            for (int k = LOG; k >= 0; --k)
                if (up[k][v] != -1 && val[up[k][v]] <= W) v = up[k][v];
            return leaves[v];
        }
    };

} // namespace krt

// Brute force: BFS restricted to edges <= W
int bfs_count(int n, const std::vector<krt::Edge>& e, int s, int W) {
    std::vector<std::vector<int>> g(n);
    for (auto& x : e) if (x.w <= W) { g[x.u].push_back(x.v); g[x.v].push_back(x.u); }
    std::vector<bool> seen(n, false);
    std::queue<int> q; q.push(s); seen[s] = true; int c = 0;
    while (!q.empty()) { int u = q.front(); q.pop(); ++c; for (int v : g[u]) if (!seen[v]) { seen[v] = true; q.push(v); } }
    return c;
}

// Brute force minimax: smallest W such that v is reachable from u
int brute_minimax(int n, const std::vector<krt::Edge>& e, int u, int v) {
    if (u == v) return 0;
    std::vector<int> ws; for (auto& x : e) ws.push_back(x.w);
    std::sort(ws.begin(), ws.end());
    for (int W : ws) {
        std::vector<std::vector<int>> g(n);
        for (auto& x : e) if (x.w <= W) { g[x.u].push_back(x.v); g[x.v].push_back(x.u); }
        std::vector<bool> seen(n, false); std::queue<int> q; q.push(u); seen[u] = true;
        while (!q.empty()) { int a = q.front(); q.pop(); for (int b : g[a]) if (!seen[b]) { seen[b] = true; q.push(b); } }
        if (seen[v]) return W;
    }
    return -1;
}

int main() {
    std::cout << "============================================================\n";
    std::cout << "               Kruskal Reconstruction Tree (KRT)\n";
    std::cout << "============================================================\n\n";

    std::vector<krt::Edge> edges = {{0,1,4},{1,2,2},{2,3,7},{0,3,9},{3,4,1},{4,5,6},{2,5,3}};
    krt::KruskalTree t(6, edges);
    std::cout << "Graph: 6 vertices, edges (u,v,w): ";
    for (auto& e : edges) std::cout << "(" << e.u << "," << e.v << "," << e.w << ") ";
    std::cout << "\n\nMinimax path weights:\n";
    for (auto [u, v] : std::vector<std::pair<int,int>>{{0,4},{1,5},{3,5},{0,2}})
        std::cout << "  minimax(" << u << ", " << v << ") = " << t.minimax(u, v) << "\n";
    std::cout << "\nReachable from vertex 0 with edges <= W:\n";
    for (int W : {0, 3, 4, 6, 10}) std::cout << "  W = " << W << ": " << t.reachable(0, W) << " vertices\n";

    std::mt19937 rng(321);
    bool ok = true;
    for (int trial = 0; trial < 40 && ok; ++trial) {
        int n = 2 + rng() % 25, m = rng() % (2 * n);
        std::vector<krt::Edge> e;
        for (int i = 0; i < m; ++i) e.push_back({int(rng() % n), int(rng() % n), int(1 + rng() % 50)});
        krt::KruskalTree k(n, e);
        for (int q = 0; q < 40 && ok; ++q) {
            int u = rng() % n, v = rng() % n, W = rng() % 55;
            if (k.minimax(u, v) != brute_minimax(n, e, u, v)) ok = false;
            if (k.reachable(u, W) != bfs_count(n, e, u, W)) ok = false;
        }
    }
    std::cout << "\nRandom graphs vs brute force (minimax + reachability): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
