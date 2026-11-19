/*
 * ============================================================
 *            Matroid Intersection Algorithm
 * ============================================================
 *
 * Description:
 *   Given two matroids M1 = (E, I1) and M2 = (E, I2) on the
 *   same ground set E, the matroid intersection problem finds
 *   a maximum cardinality set S that is independent in both
 *   matroids simultaneously (S in I1 ∩ I2).
 *
 *   This is a powerful generalization that captures:
 *   - Bipartite matching (intersection of two partition matroids)
 *   - Arborescences (graphic + partition matroid)
 *   - Colorful spanning trees (graphic + partition matroid)
 *
 * Algorithm (Augmenting Path Method):
 *   Starting with S = {} (empty set, independent in both):
 *   1. Build an exchange graph D_S on ground set E:
 *      - For e in S, f not in S: edge (e,f) if
 *        (S - e + f) in I1
 *      - For f not in S, e in S: edge (f,e) if
 *        (S - e + f) in I2
 *   2. Let X1 = {e not in S : S + e in I1}
 *          X2 = {e not in S : S + e in I2}
 *   3. Find shortest path from X1 to X2 in D_S (via BFS)
 *   4. If path exists: augment S by symmetric difference
 *      with path elements (add/remove alternately)
 *   5. Repeat until no augmenting path exists
 *
 * Matroid Oracles Used:
 *   - Graphic matroid: S is independent iff it is acyclic
 *   - Partition matroid: S is independent iff it has at most
 *     k_i elements from each partition class
 *
 * Complexity:
 *   Time:  O(r * |E|^2 * T_oracle) where r is the rank
 *   Space: O(|E|^2)
 *
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iomanip>
#include <set>

namespace matroid_intersection {

    // A matroid is defined by its independence oracle
    // oracle(S) returns true if S is independent
    // We also need an incremental oracle: given independent set S and element e,
    // is S + e independent?
    struct Matroid {
        int ground_size;
        // Check if adding element e to independent set S keeps it independent
        std::function<bool(const std::vector<bool>&, int)> can_add;
        // Check if replacing element e_out with e_in keeps independence
        std::function<bool(const std::vector<bool>&, int, int)> can_exchange;
    };

    // ---- Graphic Matroid ----
    // Ground set = edges, independent = acyclic (forest)
    struct GraphicMatroid {
        int n_vertices;
        std::vector<std::pair<int,int>> edges;

        // Union-Find for cycle detection
        struct UF {
            std::vector<int> parent, rank_;
            UF(int n) : parent(n), rank_(n, 0) {
                std::iota(parent.begin(), parent.end(), 0);
            }
            int find(int x) {
                while (parent[x] != x) x = parent[x] = parent[parent[x]];
                return x;
            }
            bool unite(int a, int b) {
                a = find(a); b = find(b);
                if (a == b) return false;
                if (rank_[a] < rank_[b]) std::swap(a, b);
                parent[b] = a;
                if (rank_[a] == rank_[b]) rank_[a]++;
                return true;
            }
        };

        // Build UF from current set (excluding one edge optionally)
        UF build_uf(const std::vector<bool>& in_set, int exclude = -1) const {
            UF uf(n_vertices);
            for (int i = 0; i < static_cast<int>(edges.size()); ++i) {
                if (i == exclude) continue;
                if (in_set[i]) {
                    uf.unite(edges[i].first, edges[i].second);
                }
            }
            return uf;
        }

        Matroid to_matroid() const {
            Matroid m;
            m.ground_size = static_cast<int>(edges.size());
            m.can_add = [this](const std::vector<bool>& S, int e) -> bool {
                auto uf = build_uf(S);
                return uf.find(edges[e].first) != uf.find(edges[e].second);
            };
            m.can_exchange = [this](const std::vector<bool>& S, int e_out, int e_in) -> bool {
                auto uf = build_uf(S, e_out);
                return uf.find(edges[e_in].first) != uf.find(edges[e_in].second);
            };
            return m;
        }
    };

    // ---- Partition Matroid ----
    // Elements partitioned into groups, at most k_i from group i
    struct PartitionMatroid {
        int ground_size;
        std::vector<int> group;        // group[e] = which group element e belongs to
        std::vector<int> capacity;     // capacity[g] = max elements from group g

        Matroid to_matroid() const {
            Matroid m;
            m.ground_size = ground_size;
            m.can_add = [this](const std::vector<bool>& S, int e) -> bool {
                int g = group[e];
                int count = 0;
                for (int i = 0; i < ground_size; ++i) {
                    if (S[i] && group[i] == g) ++count;
                }
                return count < capacity[g];
            };
            m.can_exchange = [this](const std::vector<bool>& S, int e_out, int e_in) -> bool {
                int g_in = group[e_in];
                int count = 0;
                for (int i = 0; i < ground_size; ++i) {
                    if (i == e_out) continue;
                    if (S[i] && group[i] == g_in) ++count;
                }
                return count < capacity[g_in];
            };
            return m;
        }
    };

    // ---- Matroid Intersection Algorithm ----
    // Returns maximum common independent set as a boolean vector
    std::vector<bool> matroid_intersect(const Matroid& m1, const Matroid& m2, int n) {
        std::vector<bool> S(n, false);  // current solution

        while (true) {
            // Find X1 = {e not in S : S + e in I1}
            // Find X2 = {e not in S : S + e in I2}
            std::vector<bool> in_X1(n, false), in_X2(n, false);
            for (int e = 0; e < n; ++e) {
                if (!S[e]) {
                    if (m1.can_add(S, e)) in_X1[e] = true;
                    if (m2.can_add(S, e)) in_X2[e] = true;
                }
            }

            // Build exchange graph and BFS from X1 to X2
            // Nodes: elements 0..n-1
            // Edges:
            //   For e in S, f not in S: (e -> f) if (S - e + f) in I1
            //   For f not in S, e in S: (f -> e) if (S - e + f) in I2
            std::vector<int> parent(n, -2);  // -2 = unvisited, -1 = source
            std::queue<int> bfs_queue;

            // Start BFS from X1 elements
            for (int e = 0; e < n; ++e) {
                if (in_X1[e]) {
                    parent[e] = -1;
                    bfs_queue.push(e);
                }
            }

            int target = -1;
            while (!bfs_queue.empty() && target == -1) {
                int u = bfs_queue.front();
                bfs_queue.pop();

                if (S[u]) {
                    // u is in S: edges to f not in S where (S - u + f) in I1
                    for (int f = 0; f < n; ++f) {
                        if (!S[f] && parent[f] == -2) {
                            if (m1.can_exchange(S, u, f)) {
                                parent[f] = u;
                                if (in_X2[f]) {
                                    target = f;
                                    break;
                                }
                                bfs_queue.push(f);
                            }
                        }
                    }
                } else {
                    // u is not in S: edges to e in S where (S - e + u) in I2
                    for (int e = 0; e < n; ++e) {
                        if (S[e] && parent[e] == -2) {
                            if (m2.can_exchange(S, e, u)) {
                                parent[e] = u;
                                bfs_queue.push(e);
                            }
                        }
                    }
                }
            }

            if (target == -1) break;  // No augmenting path

            // Augment: toggle elements along the path
            int cur = target;
            while (cur != -1) {
                S[cur] = !S[cur];
                cur = parent[cur];
            }
        }

        return S;
    }

} // namespace matroid_intersection

int main() {
    using namespace matroid_intersection;

    std::cout << "============================================================\n";
    std::cout << "             Matroid Intersection Algorithm\n";
    std::cout << "============================================================\n\n";

    // Example 1: Bipartite Matching via Matroid Intersection
    // Left vertices {0,1,2}, Right vertices {3,4,5}
    // Edges: 0-3, 0-4, 1-3, 1-5, 2-4, 2-5
    // M1 = partition matroid on left side (at most 1 per left vertex)
    // M2 = partition matroid on right side (at most 1 per right vertex)
    {
        std::cout << "--- Example 1: Bipartite Matching ---\n";
        std::cout << "Left: {A, B, C}, Right: {X, Y, Z}\n";
        std::cout << "Edges: A-X, A-Y, B-X, B-Z, C-Y, C-Z\n\n";

        int n_edges = 6;
        // Edge list: (left, right)
        // 0: A-X, 1: A-Y, 2: B-X, 3: B-Z, 4: C-Y, 5: C-Z
        std::vector<std::string> edge_names = {"A-X", "A-Y", "B-X", "B-Z", "C-Y", "C-Z"};
        std::vector<int> left_group  = {0, 0, 1, 1, 2, 2};  // which left vertex
        std::vector<int> right_group = {0, 1, 0, 2, 1, 2};  // which right vertex

        PartitionMatroid pm1;
        pm1.ground_size = n_edges;
        pm1.group = left_group;
        pm1.capacity = {1, 1, 1};  // at most 1 edge per left vertex

        PartitionMatroid pm2;
        pm2.ground_size = n_edges;
        pm2.group = right_group;
        pm2.capacity = {1, 1, 1};  // at most 1 edge per right vertex

        auto m1 = pm1.to_matroid();
        auto m2 = pm2.to_matroid();

        auto result = matroid_intersect(m1, m2, n_edges);

        int matching_size = 0;
        std::cout << "Maximum matching:\n";
        for (int i = 0; i < n_edges; ++i) {
            if (result[i]) {
                std::cout << "  " << edge_names[i] << "\n";
                ++matching_size;
            }
        }
        std::cout << "Matching size: " << matching_size << " (expected: 3)\n";
    }

    // Example 2: Colorful Spanning Forest
    // Graph with colored edges; find spanning forest using at most 1 edge per color
    {
        std::cout << "\n--- Example 2: Colorful Spanning Forest ---\n";
        std::cout << "Graph: 4 vertices, 6 edges with 3 colors\n";

        // Vertices: 0, 1, 2, 3
        // Edges: (0,1) red, (0,2) blue, (0,3) green,
        //        (1,2) green, (1,3) blue, (2,3) red
        GraphicMatroid gm;
        gm.n_vertices = 4;
        gm.edges = {{0,1}, {0,2}, {0,3}, {1,2}, {1,3}, {2,3}};
        int n_edges = 6;

        std::vector<std::string> edge_labels = {
            "0-1(red)", "0-2(blue)", "0-3(green)",
            "1-2(green)", "1-3(blue)", "2-3(red)"
        };
        std::vector<int> colors = {0, 1, 2, 2, 1, 0};  // red=0, blue=1, green=2

        // M1 = graphic matroid (acyclic)
        auto m1 = gm.to_matroid();

        // M2 = partition matroid (at most 1 edge per color)
        PartitionMatroid pm;
        pm.ground_size = n_edges;
        pm.group = colors;
        pm.capacity = {1, 1, 1};
        auto m2 = pm.to_matroid();

        auto result = matroid_intersect(m1, m2, n_edges);

        int forest_size = 0;
        std::cout << "Colorful spanning forest:\n";
        for (int i = 0; i < n_edges; ++i) {
            if (result[i]) {
                std::cout << "  " << edge_labels[i] << "\n";
                ++forest_size;
            }
        }
        std::cout << "Forest size: " << forest_size
                  << " (max spanning tree has " << (gm.n_vertices - 1) << " edges)\n";
    }

    // Example 3: Larger bipartite matching
    {
        std::cout << "\n--- Example 3: Larger Bipartite Matching (5 x 5) ---\n";

        // 5 left, 5 right, various edges
        // Edges represented as (left_vertex, right_vertex)
        std::vector<std::pair<int,int>> bip_edges = {
            {0,0}, {0,1}, {1,1}, {1,2}, {2,2}, {2,3}, {3,3}, {3,4}, {4,0}, {4,4}
        };
        int n_edges = static_cast<int>(bip_edges.size());

        std::vector<int> lg(n_edges), rg(n_edges);
        for (int i = 0; i < n_edges; ++i) {
            lg[i] = bip_edges[i].first;
            rg[i] = bip_edges[i].second;
        }

        PartitionMatroid pm1;
        pm1.ground_size = n_edges;
        pm1.group = lg;
        pm1.capacity = {1, 1, 1, 1, 1};

        PartitionMatroid pm2;
        pm2.ground_size = n_edges;
        pm2.group = rg;
        pm2.capacity = {1, 1, 1, 1, 1};

        auto result = matroid_intersect(pm1.to_matroid(), pm2.to_matroid(), n_edges);

        int matching_size = 0;
        std::cout << "Matching edges: ";
        for (int i = 0; i < n_edges; ++i) {
            if (result[i]) {
                std::cout << "L" << bip_edges[i].first << "-R" << bip_edges[i].second << " ";
                ++matching_size;
            }
        }
        std::cout << "\nMatching size: " << matching_size << " (expected: 5)\n";
    }

    // Verification: brute force for Example 1
    {
        std::cout << "\n--- Brute Force Verification (Example 1) ---\n";
        int n = 6;
        int best = 0;
        // Check all subsets
        for (int mask = 0; mask < (1 << n); ++mask) {
            std::vector<bool> S(n, false);
            for (int i = 0; i < n; ++i) {
                if (mask & (1 << i)) S[i] = true;
            }

            // Check partition matroid 1 (left side)
            std::vector<int> left_group  = {0, 0, 1, 1, 2, 2};
            std::vector<int> right_group = {0, 1, 0, 2, 1, 2};
            int left_counts[3] = {}, right_counts[3] = {};
            bool valid = true;
            int count = 0;
            for (int i = 0; i < n; ++i) {
                if (S[i]) {
                    ++count;
                    if (++left_counts[left_group[i]] > 1) { valid = false; break; }
                    if (++right_counts[right_group[i]] > 1) { valid = false; break; }
                }
            }
            if (valid) best = std::max(best, count);
        }
        std::cout << "Brute force max matching: " << best << " (expected: 3)\n";
    }

    std::cout << "\n============================================================\n";
    std::cout << "  Matroid intersection generalizes bipartite matching,\n";
    std::cout << "  colorful spanning trees, and many other combinatorial\n";
    std::cout << "  optimization problems.\n";
    std::cout << "============================================================\n";

    return 0;
}
