/*
 * ============================================================
 *       All-Roots Tree DP (Generic Rerooting Technique)
 * ============================================================
 *
 * Description:
 *   When a tree DP asks for the answer assuming EVERY vertex
 *   r = 0, 1, ..., N-1 is the root, the two-DFS rerooting
 *   technique solves all N roots in O(N) time without needing a
 *   subtraction / division inverse:
 *
 *     1. Bottom-Up DFS (from arbitrary root 0):
 *        Compute down[u] = aggregate of u's subtree.
 *     2. Top-Down DFS:
 *        At vertex u, collect the edge-transitioned values from
 *        all neighbors of u (parent's up[u] + every child's down[v]).
 *        Build prefix and suffix combinations of this list so that:
 *          up[v_i] = transition(merge(prefix[i-1], suffix[i+1]), edge(u, v_i))
 *        in O(deg(u)) without ever dividing or subtracting!
 *
 *   Demonstrated on two simultaneous queries for every root r:
 *     - sum_dist[r]: sum of weighted distances from r to all nodes
 *     - eccentricity[r]: maximum weighted distance from r (which
 *       has NO inverse operation, proving prefix/suffix power).
 *
 * Complexity:
 *   Time:  O(N), Space: O(N).
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <iomanip>

namespace rerooting {

    struct Aggregate {
        int count = 1;           // number of vertices in side
        long long sum_dist = 0;  // sum of distances to side root
        long long max_dist = 0;  // longest path inside side
    };

    // Lift a child subtree's aggregate across an edge of weight w into its parent
    Aggregate add_edge_contribution(const Aggregate& child, long long w) {
        return {
            child.count,
            child.sum_dist + child.count * w,
            child.max_dist + w
        };
    }

    // Merge two edge-lifted neighbor contributions at the same vertex
    Aggregate combine(const Aggregate& a, const Aggregate& b) {
        return {
            a.count + b.count,
            a.sum_dist + b.sum_dist,
            std::max(a.max_dist, b.max_dist)
        };
    }

    class RerootingSolver {
        struct Edge { int to; long long w; };
        int n;
        std::vector<std::vector<Edge>> adj;
        std::vector<Aggregate> down, full;

        void dfs_down(int u, int p) {
            Aggregate cur{};
            for (auto [v, w] : adj[u]) {
                if (v == p) continue;
                dfs_down(v, u);
                cur = combine(cur, add_edge_contribution(down[v], w));
            }
            down[u] = cur;
        }

        void dfs_up(int u, int p, const Aggregate& from_parent) {
            // Gather all lifted contributions incident to u
            std::vector<Edge> children;
            std::vector<Aggregate> contrib;
            if (p != -1) contrib.push_back(from_parent);
            for (auto [v, w] : adj[u]) {
                if (v == p) continue;
                children.push_back({v, w});
                contrib.push_back(add_edge_contribution(down[v], w));
            }

            int m = static_cast<int>(contrib.size());
            std::vector<Aggregate> pref(m + 1), suff(m + 1);
            for (int i = 0; i < m; ++i) pref[i + 1] = combine(pref[i], contrib[i]);
            for (int i = m - 1; i >= 0; --i) suff[i] = combine(contrib[i], suff[i + 1]);

            full[u] = pref[m];
            int offset = (p != -1) ? 1 : 0;
            for (int i = 0; i < static_cast<int>(children.size()); ++i) {
                int idx = i + offset;
                // Merge everything except contrib[idx]; subtract 1 extra vertex count from identity
                Aggregate without_child{
                    pref[idx].count + suff[idx + 1].count - 1,
                    pref[idx].sum_dist + suff[idx + 1].sum_dist,
                    std::max(pref[idx].max_dist, suff[idx + 1].max_dist)
                };
                dfs_up(children[i].to, u, add_edge_contribution(without_child, children[i].w));
            }
        }

    public:
        explicit RerootingSolver(int n) : n(n), adj(n), down(n), full(n) {}

        void add_edge(int u, int v, long long w) {
            adj[u].push_back({v, w});
            adj[v].push_back({u, w});
        }

        const std::vector<Aggregate>& solve() {
            dfs_down(0, -1);
            dfs_up(0, -1, Aggregate{});
            return full;
        }

        Aggregate brute_from(int root) const {
            auto dfs = [&](auto& self, int u, int p) -> Aggregate {
                Aggregate cur{};
                for (auto [v, w] : adj[u])
                    if (v != p) cur = combine(cur, add_edge_contribution(self(self, v, u), w));
                return cur;
            };
            return dfs(dfs, root, -1);
        }
    };

} // namespace rerooting

int main() {
    using namespace rerooting;
    std::cout << "============================================================\n";
    std::cout << "       All-Roots Tree DP (Generic Rerooting Technique)\n";
    std::cout << "============================================================\n\n";

    RerootingSolver demo(6);
    demo.add_edge(0, 1, 2); demo.add_edge(0, 2, 5);
    demo.add_edge(1, 3, 4); demo.add_edge(1, 4, 1);
    demo.add_edge(2, 5, 3);
    const auto& ans = demo.solve();

    std::cout << "Vertex   SumDist   Eccentricity (MaxDist)\n";
    for (int v = 0; v < 6; ++v)
        std::cout << "  " << v << "      " << std::setw(6) << ans[v].sum_dist
                  << "          " << ans[v].max_dist << "\n";

    std::mt19937 rng(4242);
    const int N = 600;
    RerootingSolver big(N);
    for (int v = 1; v < N; ++v) big.add_edge(v, rng() % v, 1 + rng() % 100);
    const auto& res = big.solve();
    bool ok = true;
    for (int v = 0; v < N && ok; ++v) {
        auto b = big.brute_from(v);
        if (b.sum_dist != res[v].sum_dist || b.max_dist != res[v].max_dist) ok = false;
    }

    std::cout << "\nRandom weighted tree (N=600) O(N) rerooting vs O(N^2) all-roots DFS: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
