/*
 * ============================================================
 *     Maximum Weight Closure of a Digraph (Project Selection)
 * ============================================================
 *
 * Description:
 *   Given a directed graph G = (V, E) with vertex weights w(v)
 *   (positive = profit of a project, negative = cost of a
 *   prerequisite resource), a CLOSURE is a subset C of V such
 *   that every outgoing dependency edge from u in C also lands
 *   in C (u in C and (u -> v) in E  =>  v in C).
 *
 *   Picard's theorem (1976) reduces Maximum Weight Closure to
 *   Minimum s-t Cut:
 *     - For each v with w(v) > 0: add edge (s -> v) of capacity w(v).
 *     - For each v with w(v) < 0: add edge (v -> t) of capacity -w(v).
 *     - For each dependency (u -> v) in E: add edge (u -> v) of
 *       capacity +infinity.
 *
 *   Then:
 *     max_closure_weight = (sum_{w(v) > 0} w(v)) - min_cut(s, t)
 *   and the optimal closure C is the set of vertices reachable
 *   from s in the residual network.
 *
 * Complexity:
 *   O(V^2 E) worst-case with Dinic's algorithm (much faster in
 *   practice on project-dependency networks).
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <random>
#include <algorithm>

namespace weight_closure {

    class MaxClosureSolver {
        struct Edge { int to, rev; long long cap; };
        int n;
        std::vector<long long> w;
        std::vector<std::pair<int, int>> deps;

    public:
        struct Solution {
            long long max_weight;
            std::vector<int> chosen;
        };

        explicit MaxClosureSolver(std::vector<long long> weights)
            : n(static_cast<int>(weights.size())), w(std::move(weights)) {}

        // If project u is chosen, prerequisite v must also be chosen
        void add_dependency(int u, int v) { deps.push_back({u, v}); }

        Solution solve() const {
            const long long INF = LLONG_MAX / 4;
            int S = n, T = n + 1, V = n + 2;
            std::vector<std::vector<Edge>> g(V);
            auto add_edge = [&](int u, int v, long long c) {
                g[u].push_back({v, static_cast<int>(g[v].size()), c});
                g[v].push_back({u, static_cast<int>(g[u].size()) - 1, 0});
            };

            long long pos_sum = 0;
            for (int i = 0; i < n; ++i) {
                if (w[i] > 0)      { pos_sum += w[i]; add_edge(S, i, w[i]); }
                else if (w[i] < 0) { add_edge(i, T, -w[i]); }
            }
            for (auto [u, v] : deps) add_edge(u, v, INF);

            std::vector<int> level(V), ptr(V);
            auto bfs = [&]() {
                std::fill(level.begin(), level.end(), -1);
                std::queue<int> q;
                level[S] = 0; q.push(S);
                while (!q.empty()) {
                    int u = q.front(); q.pop();
                    for (const auto& e : g[u])
                        if (e.cap > 0 && level[e.to] == -1) {
                            level[e.to] = level[u] + 1;
                            q.push(e.to);
                        }
                }
                return level[T] != -1;
            };

            auto dfs = [&](auto& self, int u, long long pushed) -> long long {
                if (pushed == 0 || u == T) return pushed;
                for (int& cid = ptr[u]; cid < static_cast<int>(g[u].size()); ++cid) {
                    auto& e = g[u][cid];
                    if (e.cap > 0 && level[e.to] == level[u] + 1) {
                        long long tr = self(self, e.to, std::min(pushed, e.cap));
                        if (tr > 0) {
                            e.cap -= tr;
                            g[e.to][e.rev].cap += tr;
                            return tr;
                        }
                    }
                }
                return 0;
            };

            long long flow = 0;
            while (bfs()) {
                std::fill(ptr.begin(), ptr.end(), 0);
                while (long long pushed = dfs(dfs, S, INF)) flow += pushed;
            }

            // Recover s-side of minimum cut via residual reachability
            std::vector<int> chosen;
            for (int i = 0; i < n; ++i) if (level[i] != -1) chosen.push_back(i);
            return {pos_sum - flow, chosen};
        }
    };

} // namespace weight_closure

int main() {
    using namespace weight_closure;
    std::cout << "============================================================\n";
    std::cout << "     Maximum Weight Closure of a Digraph (Project Selection)\n";
    std::cout << "============================================================\n\n";

    // 3 projects (0..2) with profits +10, +15, +20; 3 machines (3..5) with costs -8, -12, -14
    MaxClosureSolver demo({10, 15, 20, -8, -12, -14});
    demo.add_dependency(0, 3); demo.add_dependency(0, 4);
    demo.add_dependency(1, 4); demo.add_dependency(1, 5);
    demo.add_dependency(2, 5);
    auto sol = demo.solve();
    std::cout << "Optimal net profit = " << sol.max_weight << "\nChosen nodes: ";
    for (int v : sol.chosen) std::cout << v << " ";
    std::cout << "\n";

    // Brute-force verification on random instances
    std::mt19937 rng(9001);
    bool ok = true;
    for (int trial = 0; trial < 150 && ok; ++trial) {
        int n = 4 + rng() % 11;
        std::vector<long long> w(n);
        for (auto& x : w) x = static_cast<long long>(rng() % 61) - 30;
        MaxClosureSolver solver(w);
        int m = rng() % (2 * n);
        std::vector<std::pair<int, int>> edges;
        for (int i = 0; i < m; ++i) {
            int u = rng() % n, v = rng() % n;
            if (u != v) { solver.add_dependency(u, v); edges.push_back({u, v}); }
        }
        auto got = solver.solve();

        // Check that returned set is a valid closure and achieves got.max_weight
        int mask_got = 0;
        long long sum_got = 0;
        for (int v : got.chosen) { mask_got |= (1 << v); sum_got += w[v]; }
        for (auto [u, v] : edges)
            if ((mask_got & (1 << u)) && !(mask_got & (1 << v))) ok = false;
        if (sum_got != got.max_weight) ok = false;

        long long best = 0;
        for (int mask = 0; mask < (1 << n); ++mask) {
            bool valid = true;
            for (auto [u, v] : edges)
                if ((mask & (1 << u)) && !(mask & (1 << v))) { valid = false; break; }
            if (!valid) continue;
            long long s = 0;
            for (int i = 0; i < n; ++i) if (mask & (1 << i)) s += w[i];
            best = std::max(best, s);
        }
        if (got.max_weight != best) ok = false;
    }
    std::cout << "\nRandom digraphs vs 2^N subset enumeration (150 trials): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
