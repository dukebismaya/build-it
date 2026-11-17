/**
 * ============================================================================
 * Chu-Liu / Edmonds' Algorithm (Minimum Weight Directed Arborescence)
 * ============================================================================
 * 
 * Description:
 * Given a directed graph with edge weights and a specified root vertex r,
 * find a directed spanning tree rooted at r (a branching/arborescence where
 * every vertex except r has in-degree exactly 1, and there exists a unique
 * directed path from r to every other vertex) with the minimum total weight.
 * 
 * Chu-Liu / Edmonds' Algorithm (1965 / 1967):
 * 1. Minimum Incoming Edges:
 *    For every vertex v != r, select the minimum-weight incoming edge min_in[v].
 *    If any reachable vertex has no incoming edge, no valid arborescence exists.
 * 
 * 2. Cycle Detection:
 *    Traverse the chosen incoming edges to identify directed cycles.
 *    If no cycles are detected, the chosen edges form a valid minimum
 *    arborescence, and we terminate!
 * 
 * 3. Cycle Contraction & Edge Reweighting:
 *    If a directed cycle C is found:
 *    - Contract all vertices in C into a single supernode.
 *    - For every external edge u -> v entering the cycle (u not in C, v in C):
 *        reweight(u -> v) = weight(u -> v) - min_in[v].
 *      (This accounts for replacing the cycle's internal edge entering v).
 *    - Add the sum of cycle edges sum_{v in C} min_in[v] to the total.
 *    - Recurse on the contracted graph with fewer vertices.
 * 
 * Time Complexity:
 * - O(V * E) runtime with adjacency / edge list contractions
 * - Space Complexity: O(V + E)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <algorithm>

namespace chu_liu_edmonds {

constexpr int64_t INF = std::numeric_limits<int64_t>::max() / 4;

struct Edge {
    int u, v;
    int64_t weight;
};

/**
 * Computes minimum directed spanning tree weight rooted at root.
 * Returns -1 if no arborescence is possible.
 */
int64_t minArborescence(int n, int root, std::vector<Edge> edges) {
    int64_t total_weight = 0;

    while (true) {
        // Step 1: Find minimum incoming edge for each vertex (except root)
        std::vector<int64_t> in_weight(n, INF);
        std::vector<int> parent(n, -1);

        for (const auto& e : edges) {
            if (e.u != e.v && e.v != root && e.weight < in_weight[e.v]) {
                in_weight[e.v] = e.weight;
                parent[e.v] = e.u;
            }
        }

        // Check reachability
        for (int i = 0; i < n; ++i) {
            if (i != root && in_weight[i] == INF) {
                return -1; // Unreachable node
            }
        }

        // Step 2: Detect directed cycles
        std::vector<int> cycle_id(n, -1);
        std::vector<int> visited(n, -1);
        int cycle_count = 0;

        for (int i = 0; i < n; ++i) {
            if (i == root) continue;
            total_weight += in_weight[i];

            int curr = i;
            while (curr != root && visited[curr] == -1 && cycle_id[curr] == -1) {
                visited[curr] = i;
                curr = parent[curr];
            }

            if (curr != root && visited[curr] == i) {
                // Detected a cycle
                int v = curr;
                while (cycle_id[v] == -1) {
                    cycle_id[v] = cycle_count;
                    v = parent[v];
                }
                cycle_count++;
            }
        }

        // If no cycles detected, the arborescence is complete!
        if (cycle_count == 0) {
            break;
        }

        // Assign IDs to vertices not part of any cycle
        for (int i = 0; i < n; ++i) {
            if (cycle_id[i] == -1) {
                cycle_id[i] = cycle_count++;
            }
        }

        // Step 3: Contract cycles and reweight edges
        std::vector<Edge> new_edges;
        for (const auto& e : edges) {
            int u_new = cycle_id[e.u];
            int v_new = cycle_id[e.v];
            if (u_new != v_new) {
                // If entering a cycle node v, reweight by subtracting in_weight[v]
                int64_t w_new = e.weight - in_weight[e.v];
                new_edges.push_back({u_new, v_new, w_new});
            }
        }

        n = cycle_count;
        root = cycle_id[root];
        edges = std::move(new_edges);
    }

    return total_weight;
}

} // namespace chu_liu_edmonds

int main() {
    std::cout << "========================================================\n";
    std::cout << " Chu-Liu / Edmonds' Minimum Directed Arborescence\n";
    std::cout << "========================================================\n\n";

    // Graph 1: Simple 4-node graph
    // 0 -> 1 (w=4), 0 -> 2 (w=2), 1 -> 2 (w=1), 2 -> 1 (w=3), 2 -> 3 (w=5), 1 -> 3 (w=4)
    int n1 = 4;
    int root1 = 0;
    std::vector<chu_liu_edmonds::Edge> edges1 = {
        {0, 1, 4},
        {0, 2, 2},
        {1, 2, 1},
        {2, 1, 3},
        {2, 3, 5},
        {1, 3, 4}
    };

    std::cout << "Graph 1 (" << n1 << " nodes, root = " << root1 << "):\n";
    for (const auto& e : edges1) {
        std::cout << "  " << e.u << " -> " << e.v << " (weight = " << e.weight << ")\n";
    }

    int64_t cost1 = chu_liu_edmonds::minArborescence(n1, root1, edges1);
    std::cout << "\nMinimum Arborescence Cost: " << cost1 << "\n";
    // Expected: 0->2 (2) + 2->1 (3) + 1->3 (4) = 9
    std::cout << "Expected Cost: 9 -> " << (cost1 == 9 ? "PASSED" : "FAILED") << "\n\n";

    // Graph 2: Graph with a cycle that requires contraction
    // Cycle between 1, 2, 3
    int n2 = 5;
    int root2 = 0;
    std::vector<chu_liu_edmonds::Edge> edges2 = {
        {0, 1, 10},
        {0, 2, 15},
        {1, 2, 1},
        {2, 3, 1},
        {3, 1, 1}, // Cycle 1->2->3->1 (cost 3)
        {2, 4, 3},
        {3, 4, 2}
    };

    std::cout << "--------------------------------------------------------\n";
    std::cout << "Graph 2 (" << n2 << " nodes with cycle 1-2-3-1, root = " << root2 << "):\n";
    int64_t cost2 = chu_liu_edmonds::minArborescence(n2, root2, edges2);
    std::cout << "Minimum Arborescence Cost: " << cost2 << "\n";
    // Expected: 0->1 (10) + 1->2 (1) + 2->3 (1) + 3->4 (2) = 14
    std::cout << "Expected Cost: 14 -> " << (cost2 == 14 ? "PASSED" : "FAILED") << "\n";

    return 0;
}
