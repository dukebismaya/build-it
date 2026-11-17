/**
 * ============================================================================
 * Shortest Path Faster Algorithm (SPFA) with SLF Optimization & Cycle Detection
 * ============================================================================
 * 
 * Description:
 * The Shortest Path Faster Algorithm (SPFA) is an improvement of the Bellman-Ford
 * algorithm for single-source shortest paths on directed graphs with arbitrary
 * (including negative) edge weights.
 * 
 * Key Principles:
 * 1. Queue Optimization:
 *    Instead of iterating over all edges V-1 times like standard Bellman-Ford,
 *    SPFA maintains a queue of vertices whose distances have recently improved.
 *    Only outgoing edges from these vertices are relaxed.
 * 
 * 2. Small Label First (SLF) Optimization:
 *    When vertex v is inserted into the double-ended queue:
 *      if dist[v] < dist[deque.front()], push to front
 *      else push to back.
 *    SLF prioritizes exploring closer vertices first, functioning similarly
 *    to Dijkstra's greedy order.
 * 
 * 3. Negative Cycle Detection:
 *    Maintains relax_count[v] counting how many times vertex v has been relaxed.
 *    If relax_count[v] >= V for any vertex, a negative-weight cycle reachable
 *    from the source exists.
 * 
 * Time Complexity:
 * - Average: O(k * E) where k is typically 2 to 3 on random graphs
 * - Worst-case: O(V * E) on adversarial graphs
 * - Space Complexity: O(V + E)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <deque>
#include <limits>
#include <cstdint>
#include <iomanip>

namespace spfa {

constexpr int64_t INF = std::numeric_limits<int64_t>::max() / 4;

struct Edge {
    int to;
    int64_t weight;
};

struct SPFAResult {
    bool has_negative_cycle;
    std::vector<int64_t> dist;
    std::vector<int> parent;
};

class Graph {
public:
    int n;
    std::vector<std::vector<Edge>> adj;

    explicit Graph(int vertices) : n(vertices), adj(vertices) {}

    void addEdge(int u, int v, int64_t w) {
        adj[u].push_back({v, w});
    }

    /**
     * Executes SPFA with Small Label First (SLF) optimization starting from source.
     */
    SPFAResult solve(int source) const {
        std::vector<int64_t> dist(n, INF);
        std::vector<int> parent(n, -1);
        std::vector<int> relax_count(n, 0);
        std::vector<bool> in_queue(n, false);

        std::deque<int> dq;

        dist[source] = 0;
        dq.push_back(source);
        in_queue[source] = true;

        while (!dq.empty()) {
            int u = dq.front();
            dq.pop_front();
            in_queue[u] = false;

            for (const auto& edge : adj[u]) {
                int v = edge.to;
                int64_t w = edge.weight;

                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    parent[v] = u;

                    if (!in_queue[v]) {
                        relax_count[v]++;
                        if (relax_count[v] >= n) {
                            return {true, dist, parent};
                        }

                        // Small Label First (SLF) optimization
                        if (!dq.empty() && dist[v] < dist[dq.front()]) {
                            dq.push_front(v);
                        } else {
                            dq.push_back(v);
                        }
                        in_queue[v] = true;
                    }
                }
            }
        }

        return {false, dist, parent};
    }
};

} // namespace spfa

int main() {
    std::cout << "========================================================\n";
    std::cout << " Shortest Path Faster Algorithm (SPFA) with SLF\n";
    std::cout << "========================================================\n\n";

    // Graph 1: Graph with negative edge weights but NO negative cycles
    int n1 = 6;
    spfa::Graph g1(n1);
    g1.addEdge(0, 1, 4);
    g1.addEdge(0, 2, 2);
    g1.addEdge(1, 3, -1);
    g1.addEdge(2, 1, 1);
    g1.addEdge(2, 3, 3);
    g1.addEdge(2, 4, 5);
    g1.addEdge(3, 4, -2);
    g1.addEdge(4, 5, 2);

    std::cout << "Test Graph 1 (Negative weights, no negative cycles):\n";
    auto res1 = g1.solve(0);

    std::cout << "Has negative cycle? " << (res1.has_negative_cycle ? "YES" : "NO") << "\n";
    std::cout << "Shortest distances from source vertex 0:\n";
    std::cout << "------------------------------------\n";
    std::cout << std::setw(10) << "Vertex" << std::setw(16) << "Distance" << "\n";
    std::cout << "------------------------------------\n";

    for (int i = 0; i < n1; ++i) {
        std::cout << std::setw(10) << i;
        if (res1.dist[i] >= spfa::INF) {
            std::cout << std::setw(16) << "UNREACHABLE\n";
        } else {
            std::cout << std::setw(16) << res1.dist[i] << "\n";
        }
    }
    std::cout << "------------------------------------\n\n";

    // Graph 2: Graph WITH a negative cycle (1 -> 2 -> 3 -> 1 with sum < 0)
    int n2 = 4;
    spfa::Graph g2(n2);
    g2.addEdge(0, 1, 1);
    g2.addEdge(1, 2, 2);
    g2.addEdge(2, 3, -6);
    g2.addEdge(3, 1, 1); // Cycle: 1->2 (2) -> 3 (-6) -> 1 (1) = 2 - 6 + 1 = -3

    std::cout << "Test Graph 2 (Graph with negative-weight cycle):\n";
    auto res2 = g2.solve(0);
    std::cout << "Has negative cycle? " << (res2.has_negative_cycle ? "YES (DETECTED)" : "NO") << "\n";

    return 0;
}
