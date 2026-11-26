/*
 * ============================================================
 *       Ear Decomposition & Robbins' Strong Orientation
 * ============================================================
 *
 * Description:
 *   An EAR DECOMPOSITION of an undirected graph G = (V, E) is a
 *   partition of E into a simple cycle P_0 followed by simple
 *   paths ("ears") P_1, P_2, ..., P_k such that each P_i (i >= 1)
 *   has its two endpoints in P_0 U ... U P_{i-1} and no internal
 *   vertices in P_0 U ... U P_{i-1}.
 *     - Whitney (1932): G is 2-edge-connected iff it has an ear
 *       decomposition (and 2-vertex-connected iff it has an OPEN
 *       ear decomposition where each ear's endpoints are distinct).
 *     - Robbins' Theorem (1939): G has a strongly connected
 *       orientation iff G is 2-edge-connected.
 *
 *   Linear-time DFS algorithm:
 *     1. Run a DFS tree from root 0: orient tree edges downward
 *        (parent -> child) and back-edges upward (descendant ->
 *        ancestor). Check that every non-root vertex has low[v] < tin[v]
 *        (no bridges).
 *     2. Process back-edges (u -> w) in preorder of their upper
 *        endpoint w: traverse u -> parent[u] -> ... until hitting
 *        a vertex already visited by an earlier ear. That trace
 *        plus the back-edge forms the next ear!
 *
 * Complexity:
 *   Time:  O(V + E), Space: O(V + E).
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

namespace ear_decomp {

    struct EarResult {
        bool is_two_edge_connected;
        std::vector<std::vector<int>> ears;         // each ear is a sequence of vertices v_0, v_1, ..., v_r
        std::vector<std::pair<int, int>> oriented;  // strongly connected directed edges
    };

    class EarDecomposer {
        struct Edge { int to, id; };
        int n, m = 0, timer = 0;
        std::vector<std::vector<Edge>> adj;
        std::vector<int> tin, low, parent_v, parent_e, order;
        std::vector<bool> used_e;
        std::vector<std::vector<std::pair<int, int>>> back_from_top; // bucketed by upper endpoint
        std::vector<std::pair<int, int>> directed;

        void dfs(int u, int pe) {
            tin[u] = low[u] = ++timer;
            order.push_back(u);
            for (auto [v, id] : adj[u]) {
                if (id == pe) continue;
                if (!tin[v]) {
                    used_e[id] = true;
                    parent_v[v] = u;
                    parent_e[v] = id;
                    directed.push_back({u, v});
                    dfs(v, id);
                    low[u] = std::min(low[u], low[v]);
                } else if (tin[v] < tin[u]) {
                    used_e[id] = true;
                    low[u] = std::min(low[u], tin[v]);
                    back_from_top[v].push_back({u, id});
                    directed.push_back({u, v});
                }
            }
        }

    public:
        explicit EarDecomposer(int n) : n(n), adj(n), tin(n, 0), low(n, 0),
                                        parent_v(n, -1), parent_e(n, -1), back_from_top(n) {}

        void add_edge(int u, int v) {
            adj[u].push_back({v, m});
            adj[v].push_back({u, m});
            ++m;
        }

        EarResult decompose() {
            used_e.assign(m, false);
            dfs(0, -1);
            if (static_cast<int>(order.size()) < n) return {false, {}, {}};
            for (int v = 1; v < n; ++v)
                if (low[v] >= tin[v]) return {false, {}, {}}; // bridge above v

            std::vector<bool> in_ear(n, false);
            in_ear[0] = true;
            std::vector<std::vector<int>> ears;

            for (int top : order) {
                for (auto [bot, id] : back_from_top[top]) {
                    (void)id;
                    std::vector<int> chain{bot};
                    int cur = bot;
                    while (!in_ear[cur]) {
                        in_ear[cur] = true;
                        cur = parent_v[cur];
                        chain.push_back(cur);
                    }
                    std::reverse(chain.begin(), chain.end()); // stop -> ... -> bot
                    chain.push_back(top);                     // stop -> ... -> bot -> top
                    ears.push_back(chain);
                }
            }
            return {true, ears, directed};
        }
    };

    bool is_strongly_connected(int n, const std::vector<std::pair<int, int>>& edges) {
        auto reach_all = [&](bool rev) {
            std::vector<std::vector<int>> g(n);
            for (auto [u, v] : edges) {
                if (!rev) g[u].push_back(v);
                else      g[v].push_back(u);
            }
            std::vector<bool> vis(n, false);
            std::queue<int> q;
            q.push(0); vis[0] = true;
            int cnt = 0;
            while (!q.empty()) {
                int u = q.front(); q.pop(); ++cnt;
                for (int v : g[u]) if (!vis[v]) { vis[v] = true; q.push(v); }
            }
            return cnt == n;
        };
        return reach_all(false) && reach_all(true);
    }

} // namespace ear_decomp

int main() {
    using namespace ear_decomp;
    std::cout << "============================================================\n";
    std::cout << "       Ear Decomposition & Robbins' Strong Orientation\n";
    std::cout << "============================================================\n\n";

    // Petersen graph (10 vertices, 15 edges, 3-regular biconnected)
    int n = 10, m = 15;
    EarDecomposer ed(n);
    std::vector<std::pair<int, int>> petersen = {
        {0,1},{1,2},{2,3},{3,4},{4,0},
        {5,7},{7,9},{9,6},{6,8},{8,5},
        {0,5},{1,6},{2,7},{3,8},{4,9}
    };
    for (auto [u, v] : petersen) ed.add_edge(u, v);

    auto res = ed.decompose();
    std::cout << "Petersen graph (V=10, E=15) ear decomposition (" << res.ears.size()
              << " ears, expected E - V + 1 = " << (m - n + 1) << "):\n";
    for (size_t i = 0; i < res.ears.size(); ++i) {
        std::cout << "  P_" << i << ": ";
        for (size_t j = 0; j < res.ears[i].size(); ++j)
            std::cout << (j ? " -> " : "") << res.ears[i][j];
        std::cout << "\n";
    }

    // Verify ear decomposition invariants and Robbins' orientation
    bool ok = res.is_two_edge_connected && static_cast<int>(res.ears.size()) == m - n + 1;
    std::vector<bool> covered(n, false);
    int total_edges = 0;
    for (size_t i = 0; i < res.ears.size(); ++i) {
        const auto& ear = res.ears[i];
        total_edges += static_cast<int>(ear.size()) - 1;
        if (i == 0) {
            if (ear.front() != ear.back()) ok = false;
            for (int v : ear) covered[v] = true;
        } else {
            if (!covered[ear.front()] || !covered[ear.back()]) ok = false;
            for (size_t j = 1; j + 1 < ear.size(); ++j) {
                if (covered[ear[j]]) ok = false;
                covered[ear[j]] = true;
            }
        }
    }
    for (int v = 0; v < n; ++v) if (!covered[v]) ok = false;
    if (total_edges != m || !is_strongly_connected(n, res.oriented)) ok = false;

    // Also test a graph with a bridge to confirm rejection
    EarDecomposer bridged(4);
    bridged.add_edge(0, 1); bridged.add_edge(1, 2); bridged.add_edge(2, 0);
    bridged.add_edge(2, 3); // bridge!
    if (bridged.decompose().is_two_edge_connected) ok = false;

    std::cout << "\nEar partition invariants, Robbins orientation & bridge rejection: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
