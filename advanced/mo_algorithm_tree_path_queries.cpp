/**
 * ============================================================================
 * Mo's Algorithm on Trees (Offline Tree Path Queries in O((N + Q) * sqrt(N)))
 * ============================================================================
 * 
 * Description:
 * Solves the offline tree path query problem:
 * Given a tree of N nodes, each having an integer value, answer Q queries:
 *   "Count the number of distinct node values on the simple path between u and v."
 * 
 * Core Technique (Tree Flattening via Double-Entry Euler Tour):
 * 1. Double-Entry Euler Tour:
 *    Record node u in an array of size 2N:
 *      - tin[u]: timestamp when entering u
 *      - tout[u]: timestamp when leaving u
 * 
 * 2. Path to Range Mapping (assuming tin[u] <= tin[v]):
 *    - Case 1: LCA(u, v) == u (u is an ancestor of v):
 *        The path corresponds to nodes that appear with odd frequency in
 *        the tour range [tin[u], tin[v]].
 *    - Case 2: LCA(u, v) != u:
 *        The path corresponds to nodes that appear with odd frequency in
 *        the tour range [tout[u], tin[v]], PLUS the LCA node itself!
 * 
 * 3. Parity Toggle (Inversion Property):
 *    As Mo's window [curL, curR] slides across the Euler tour:
 *      - Visiting node u for the 1st time toggles node u INTO the path (adds value).
 *      - Visiting node u for the 2nd time toggles node u OUT OF the path (removes value).
 *    A boolean array node_visited[u] tracks this parity in O(1).
 * 
 * 4. Query Reordering:
 *    Queries are sorted in blocks of size B = sqrt(2N) with Hilbert/alternating
 *    R sort order.
 * 
 * Time Complexity:
 * - Preprocessing & Euler Tour: O(N log N) with Binary Lifting LCA
 * - Query Processing: O((N + Q) * sqrt(N))
 * - Space Complexity: O(N + Q)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <iomanip>

namespace tree_mo {

struct Query {
    int id;
    int l, r;
    int lca;
    int block_id;
};

class TreeMoSolver {
public:
    int n;
    std::vector<int> values;
    std::vector<std::vector<int>> adj;

    // Euler tour and LCA
    int timer;
    std::vector<int> tin, tout;
    std::vector<int> euler;
    std::vector<int> depth;
    std::vector<std::vector<int>> up;
    int max_log;

    // Distinct counting state
    std::vector<int> freq;
    std::vector<bool> in_path;
    int distinct_count;

    TreeMoSolver(int num_nodes, const std::vector<int>& node_vals)
        : n(num_nodes), values(node_vals), adj(num_nodes), timer(0),
          tin(num_nodes), tout(num_nodes), depth(num_nodes, 0),
          distinct_count(0) {
        max_log = 1;
        while ((1 << max_log) <= n) max_log++;
        up.assign(n, std::vector<int>(max_log, 0));
        freq.assign(100005, 0); // Coordinate / frequency bound
        in_path.assign(n, false);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void dfsLCA(int u, int p, int d) {
        depth[u] = d;
        up[u][0] = p;
        for (int i = 1; i < max_log; ++i) {
            up[u][i] = up[up[u][i - 1]][i - 1];
        }

        tin[u] = timer;
        euler.push_back(u);
        timer++;

        for (int v : adj[u]) {
            if (v != p) {
                dfsLCA(v, u, d + 1);
            }
        }

        tout[u] = timer;
        euler.push_back(u);
        timer++;
    }

    int getLCA(int u, int v) const {
        if (depth[u] < depth[v]) std::swap(u, v);
        for (int i = max_log - 1; i >= 0; --i) {
            if (depth[u] - (1 << i) >= depth[v]) {
                u = up[u][i];
            }
        }
        if (u == v) return u;
        for (int i = max_log - 1; i >= 0; --i) {
            if (up[u][i] != up[v][i]) {
                u = up[u][i];
                v = up[v][i];
            }
        }
        return up[u][0];
    }

    void toggleNode(int u) {
        int val = values[u];
        if (in_path[u]) {
            // Remove node from path
            freq[val]--;
            if (freq[val] == 0) distinct_count--;
            in_path[u] = false;
        } else {
            // Add node to path
            if (freq[val] == 0) distinct_count++;
            freq[val]++;
            in_path[u] = true;
        }
    }

    std::vector<int> solveQueries(const std::vector<std::pair<int, int>>& raw_queries) {
        dfsLCA(0, 0, 0);

        int q_count = static_cast<int>(raw_queries.size());
        int euler_len = static_cast<int>(euler.size());
        int block_size = std::max(1, static_cast<int>(std::sqrt(euler_len)));

        std::vector<Query> queries(q_count);
        for (int i = 0; i < q_count; ++i) {
            int u = raw_queries[i].first;
            int v = raw_queries[i].second;
            if (tin[u] > tin[v]) std::swap(u, v);

            int lca_node = getLCA(u, v);
            queries[i].id = i;
            if (lca_node == u) {
                queries[i].l = tin[u];
                queries[i].r = tin[v];
                queries[i].lca = -1;
            } else {
                queries[i].l = tout[u];
                queries[i].r = tin[v];
                queries[i].lca = lca_node;
            }
            queries[i].block_id = queries[i].l / block_size;
        }

        std::sort(queries.begin(), queries.end(), [](const Query& a, const Query& b) {
            if (a.block_id != b.block_id) return a.block_id < b.block_id;
            return (a.block_id & 1) ? (a.r < b.r) : (a.r > b.r);
        });

        std::vector<int> answers(q_count);
        int cur_l = 0, cur_r = -1;

        for (const auto& q : queries) {
            while (cur_r < q.r) {
                cur_r++;
                toggleNode(euler[cur_r]);
            }
            while (cur_l > q.l) {
                cur_l--;
                toggleNode(euler[cur_l]);
            }
            while (cur_r > q.r) {
                toggleNode(euler[cur_r]);
                cur_r--;
            }
            while (cur_l < q.l) {
                toggleNode(euler[cur_l]);
                cur_l++;
            }

            int ans = distinct_count;
            if (q.lca != -1) {
                int lca_val = values[q.lca];
                if (freq[lca_val] == 0) ans++;
            }
            answers[q.id] = ans;
        }

        return answers;
    }
};

} // namespace tree_mo

int main() {
    std::cout << "========================================================\n";
    std::cout << " Mo's Algorithm on Trees (Distinct Value Path Queries)\n";
    std::cout << "========================================================\n\n";

    // Tree with 7 nodes (0 to 6)
    // Node values:
    // Node 0: val=1, Node 1: val=2, Node 2: val=2, Node 3: val=3
    // Node 4: val=1, Node 5: val=4, Node 6: val=3
    int n = 7;
    std::vector<int> values = {1, 2, 2, 3, 1, 4, 3};

    tree_mo::TreeMoSolver solver(n, values);
    solver.addEdge(0, 1);
    solver.addEdge(0, 2);
    solver.addEdge(1, 3);
    solver.addEdge(1, 4);
    solver.addEdge(2, 5);
    solver.addEdge(2, 6);

    std::cout << "Tree Structure (7 nodes):\n";
    for (int i = 0; i < n; ++i) {
        std::cout << "  Node " << i << " (value = " << values[i] << ")\n";
    }

    std::vector<std::pair<int, int>> queries = {
        {3, 4}, // Path: 3(val 3) - 1(val 2) - 4(val 1) -> 3 distinct
        {3, 5}, // Path: 3(3) - 1(2) - 0(1) - 2(2) - 5(4) -> {3, 2, 1, 4} = 4 distinct
        {4, 6}, // Path: 4(1) - 1(2) - 0(1) - 2(2) - 6(3) -> {1, 2, 3} = 3 distinct
        {5, 6}, // Path: 5(4) - 2(2) - 6(3) -> {4, 2, 3} = 3 distinct
        {1, 1}  // Path: 1(2) -> 1 distinct
    };

    std::cout << "\nExecuting " << queries.size() << " path queries with Tree Mo:\n";
    auto answers = solver.solveQueries(queries);

    std::cout << "--------------------------------------------------------\n";
    std::cout << std::setw(6) << "Query"
              << std::setw(16) << "Path (u <-> v)"
              << std::setw(18) << "Distinct Values\n";
    std::cout << "--------------------------------------------------------\n";

    for (size_t i = 0; i < queries.size(); ++i) {
        std::string path_str = std::to_string(queries[i].first) + " <-> " + std::to_string(queries[i].second);
        std::cout << std::setw(6) << i
                  << std::setw(16) << path_str
                  << std::setw(18) << answers[i]
                  << "\n";
    }

    std::cout << "--------------------------------------------------------\n";
    std::cout << "Tree Mo queries completed in optimal O((N + Q) * sqrt(N))!\n";

    return 0;
}
