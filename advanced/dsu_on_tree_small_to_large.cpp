/*
 * ============================================================
 *        DSU on Tree (Sack / Small-to-Large Technique)
 * ============================================================
 *
 * Description:
 *   "DSU on tree" (a.k.a. Sack, Guni) answers subtree queries
 *   such as "how many distinct colors are in the subtree of v"
 *   or "which color is dominant in subtree v" for ALL vertices
 *   offline.
 *
 * Algorithm:
 *   1. Compute subtree sizes and the heavy child of each vertex.
 *   2. Flatten the tree with an Euler tour (tin/tout, ver[]).
 *   3. dfs(v, keep):
 *        - Solve all light children with keep = false
 *          (their contribution is erased afterwards).
 *        - Solve the heavy child with keep = true
 *          (its contribution stays in the global counter).
 *        - Add v and every vertex of the light subtrees.
 *        - Record the answer for v.
 *        - If !keep, remove the entire subtree of v.
 *
 *   Each vertex is added O(log N) times because whenever it is
 *   re-added it belongs to a light subtree, whose size at least
 *   doubles on the way up.
 *
 * Complexity:
 *   O(N log N) time, O(N) space.
 *
 * Demo problem (Codeforces 600E "Lomsat gelral"):
 *   For each vertex, output the sum of all colors that appear
 *   the maximum number of times in its subtree, plus the count
 *   of distinct colors.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <random>
#include <map>

namespace sack {

    class DsuOnTree {
        int n, timer = 0;
        std::vector<std::vector<int>> adj;
        std::vector<int> color, sz, heavy, tin, tout, ver;
        std::vector<int> cnt;
        int max_freq = 0, distinct = 0;
        long long dominant_sum = 0;

        void dfs_size(int v, int p) {
            sz[v] = 1; heavy[v] = -1;
            tin[v] = timer; ver[timer++] = v;
            for (int u : adj[v]) if (u != p) {
                dfs_size(u, v);
                sz[v] += sz[u];
                if (heavy[v] == -1 || sz[u] > sz[heavy[v]]) heavy[v] = u;
            }
            tout[v] = timer;
        }

        void add(int v) {
            int c = color[v];
            if (cnt[c]++ == 0) ++distinct;
            if (cnt[c] > max_freq) { max_freq = cnt[c]; dominant_sum = c; }
            else if (cnt[c] == max_freq) dominant_sum += c;
        }

        void remove_all(int v) {
            for (int i = tin[v]; i < tout[v]; ++i) --cnt[color[ver[i]]];
            max_freq = 0; dominant_sum = 0; distinct = 0;
        }

        void dfs(int v, int p, bool keep) {
            for (int u : adj[v]) if (u != p && u != heavy[v]) dfs(u, v, false);
            if (heavy[v] != -1) dfs(heavy[v], v, true);
            add(v);
            for (int u : adj[v]) if (u != p && u != heavy[v])
                for (int i = tin[u]; i < tout[u]; ++i) add(ver[i]);
            ans_dominant[v] = dominant_sum;
            ans_distinct[v] = distinct;
            if (!keep) remove_all(v);
        }

    public:
        std::vector<long long> ans_dominant;
        std::vector<int> ans_distinct;

        DsuOnTree(int n, std::vector<int> colors, int max_color)
            : n(n), adj(n), color(std::move(colors)), sz(n), heavy(n), tin(n), tout(n),
              ver(n), cnt(max_color + 1, 0), ans_dominant(n), ans_distinct(n) {}

        void add_edge(int a, int b) { adj[a].push_back(b); adj[b].push_back(a); }

        void solve(int root = 0) {
            dfs_size(root, -1);
            dfs(root, -1, false);
        }

        // Brute-force reference using the Euler tour
        std::pair<long long, int> brute(int v) const {
            std::map<int, int> f;
            for (int i = tin[v]; i < tout[v]; ++i) ++f[color[ver[i]]];
            int best = 0; long long s = 0;
            for (auto& [c, k] : f) {
                if (k > best) { best = k; s = c; }
                else if (k == best) s += c;
            }
            return {s, static_cast<int>(f.size())};
        }
    };

} // namespace sack

int main() {
    std::cout << "============================================================\n";
    std::cout << "        DSU on Tree (Sack / Small-to-Large Technique)\n";
    std::cout << "============================================================\n\n";

    // Small example: edges and colors
    {
        std::vector<int> colors = {1, 2, 3, 2, 1, 3, 3};
        sack::DsuOnTree t(7, colors, 3);
        int edges[][2] = {{0,1},{0,2},{1,3},{1,4},{2,5},{2,6}};
        for (auto& e : edges) t.add_edge(e[0], e[1]);
        t.solve();
        std::cout << "Vertex  Color  DominantSum  Distinct\n";
        for (int v = 0; v < 7; ++v)
            std::cout << "  " << v << "       " << colors[v] << "        "
                      << t.ans_dominant[v] << "           " << t.ans_distinct[v] << "\n";
    }

    // Random verification
    std::mt19937 rng(600);
    const int N = 3000, C = 50;
    std::vector<int> colors(N);
    for (auto& c : colors) c = 1 + rng() % C;
    sack::DsuOnTree t(N, colors, C);
    for (int v = 1; v < N; ++v) t.add_edge(v, rng() % v);
    t.solve();
    bool ok = true;
    for (int v = 0; v < N && ok; ++v) {
        auto [s, d] = t.brute(v);
        if (s != t.ans_dominant[v] || d != t.ans_distinct[v]) ok = false;
    }
    std::cout << "\nRandom tree (N=" << N << ", " << C << " colors) vs brute force: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
