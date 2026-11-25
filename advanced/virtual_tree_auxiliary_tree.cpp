/*
 * ============================================================
 *       Virtual Tree (Auxiliary Tree) over Query Subsets
 * ============================================================
 *
 * Description:
 *   Given a tree T of N vertices and Q queries, where the i-th
 *   query specifies a subset S_i of K_i important vertices, the
 *   Virtual Tree (Auxiliary Tree) compresses T into a tree of at
 *   most 2*K_i - 1 vertices containing S_i and all pairwise LCAs
 *   of S_i, with edge lengths equal to original path distances.
 *
 *   Stack construction in O(K log K):
 *     1. Sort S by Euler entry time tin[v].
 *     2. Append adjacent pairwise LCAs to S, sort by tin again,
 *        and deduplicate.
 *     3. Maintain a root-to-leaf ancestor stack: for each vertex
 *        v in sorted order, pop while top is not an ancestor of v,
 *        then link top -> v in the virtual tree.
 *
 *   Demo query: Given a subset S of vertices, compute in O(|S|)
 *     - the sum of pairwise distances sum_{u < v in S} dist(u, v)
 *     - the minimum and maximum pairwise distance between two
 *       distinct vertices in S.
 *
 * Complexity:
 *   Preprocess: O(N log N) for LCA
 *   Per query of size K: O(K log K) time, O(K) virtual nodes.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <random>
#include <tuple>
#include <utility>

namespace virtual_tree {

    class TreeSolver {
        int n, LOG, timer = 0;
        std::vector<std::vector<std::pair<int, int>>> adj;
        std::vector<int> tin, tout, depth;
        std::vector<long long> dist_root;
        std::vector<std::vector<int>> up;

        // Virtual tree structures (cleared only on touched vertices)
        std::vector<std::vector<int>> vchildren;
        std::vector<bool> is_important;

        void dfs_prep(int u, int p, int d, long long w) {
            tin[u] = timer++;
            depth[u] = d;
            dist_root[u] = w;
            up[0][u] = p;
            for (int k = 1; k <= LOG; ++k) up[k][u] = up[k - 1][up[k - 1][u]];
            for (auto [v, ew] : adj[u]) if (v != p) dfs_prep(v, u, d + 1, w + ew);
            tout[u] = timer;
        }

        bool is_ancestor(int u, int v) const { return tin[u] <= tin[v] && tout[v] <= tout[u]; }

        int lca(int u, int v) const {
            if (is_ancestor(u, v)) return u;
            if (is_ancestor(v, u)) return v;
            for (int k = LOG; k >= 0; --k)
                if (!is_ancestor(up[k][u], v)) u = up[k][u];
            return up[0][u];
        }

    public:
        struct QueryAnswer {
            long long sum_dist;
            long long min_dist;
            long long max_dist;
        };

        explicit TreeSolver(int n) : n(n), adj(n), tin(n), tout(n), depth(n),
                                     dist_root(n), vchildren(n), is_important(n, false) {
            LOG = 1;
            while ((1 << LOG) <= n) ++LOG;
            up.assign(LOG + 1, std::vector<int>(n, 0));
        }

        void add_edge(int u, int v, int w) {
            adj[u].push_back({v, w});
            adj[v].push_back({u, w});
        }

        void build(int root = 0) { dfs_prep(root, root, 0, 0); }

        long long path_dist(int u, int v) const {
            return dist_root[u] + dist_root[v] - 2 * dist_root[lca(u, v)];
        }

        QueryAnswer query(std::vector<int> S) {
            int K = static_cast<int>(S.size());
            for (int u : S) is_important[u] = true;

            auto by_tin = [&](int a, int b) { return tin[a] < tin[b]; };
            std::sort(S.begin(), S.end(), by_tin);
            std::vector<int> nodes = S;
            for (int i = 0; i + 1 < K; ++i) nodes.push_back(lca(S[i], S[i + 1]));
            std::sort(nodes.begin(), nodes.end(), by_tin);
            nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());

            std::vector<int> st;
            for (int v : nodes) {
                while (!st.empty() && !is_ancestor(st.back(), v)) st.pop_back();
                if (!st.empty()) vchildren[st.back()].push_back(v);
                st.push_back(v);
            }
            int vroot = nodes[0];

            // Tree DP on virtual tree
            const long long INF = LLONG_MAX / 4;
            QueryAnswer ans{0, INF, 0};
            auto dp = [&](auto& self, int u) -> std::tuple<int, long long, long long> {
                int cnt = is_important[u] ? 1 : 0;
                long long near1 = is_important[u] ? 0 : INF;
                long long far1  = is_important[u] ? 0 : -INF;

                for (int v : vchildren[u]) {
                    long long w = dist_root[v] - dist_root[u];
                    auto [c_v, n_v, f_v] = self(self, v);
                    ans.sum_dist += w * c_v * (K - c_v);
                    if (near1 < INF / 2 && n_v < INF / 2) ans.min_dist = std::min(ans.min_dist, near1 + n_v + w);
                    if (far1 > -INF / 2 && f_v > -INF / 2) ans.max_dist = std::max(ans.max_dist, far1 + f_v + w);
                    near1 = std::min(near1, n_v + w);
                    far1  = std::max(far1, f_v + w);
                    cnt += c_v;
                }
                return {cnt, near1, far1};
            };
            dp(dp, vroot);

            for (int v : nodes) {
                vchildren[v].clear();
                is_important[v] = false;
            }
            return ans;
        }
    };

} // namespace virtual_tree

int main() {
    using namespace virtual_tree;
    std::cout << "============================================================\n";
    std::cout << "       Virtual Tree (Auxiliary Tree) over Query Subsets\n";
    std::cout << "============================================================\n\n";

    TreeSolver demo(7);
    demo.add_edge(0, 1, 3); demo.add_edge(0, 2, 4);
    demo.add_edge(1, 3, 2); demo.add_edge(1, 4, 5);
    demo.add_edge(2, 5, 1); demo.add_edge(2, 6, 6);
    demo.build();

    auto r = demo.query({3, 4, 6});
    std::cout << "Subset {3, 4, 6}: sum_dist = " << r.sum_dist
              << ", min_dist = " << r.min_dist << ", max_dist = " << r.max_dist << "\n";

    std::mt19937 rng(1337);
    const int N = 500;
    TreeSolver ts(N);
    for (int v = 1; v < N; ++v) ts.add_edge(v, rng() % v, 1 + rng() % 50);
    ts.build();

    std::vector<int> perm(N);
    for (int i = 0; i < N; ++i) perm[i] = i;
    bool ok = true;
    for (int q = 0; q < 200 && ok; ++q) {
        std::shuffle(perm.begin(), perm.end(), rng);
        int K = 2 + rng() % 30;
        std::vector<int> S(perm.begin(), perm.begin() + K);
        auto got = ts.query(S);

        long long b_sum = 0, b_min = LLONG_MAX, b_max = 0;
        for (int i = 0; i < K; ++i)
            for (int j = i + 1; j < K; ++j) {
                long long d = ts.path_dist(S[i], S[j]);
                b_sum += d;
                b_min = std::min(b_min, d);
                b_max = std::max(b_max, d);
            }
        if (got.sum_dist != b_sum || got.min_dist != b_min || got.max_dist != b_max) ok = false;
    }
    std::cout << "\nRandom verification (N=500, 200 subset queries vs O(K^2) LCA): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
