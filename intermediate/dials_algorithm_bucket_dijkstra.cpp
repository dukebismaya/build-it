/**
 * ============================================================================
 * Dial's Algorithm (Bucket Dijkstra / Integer Weight Shortest Path)
 * ============================================================================
 * 
 * Description:
 * Dial's algorithm (introduced by Robert B. Dial in 1969) is an optimization of
 * Dijkstra's shortest path algorithm for graphs with small non-negative integer
 * edge weights in the range [0, W_max].
 * 
 * Key Principles:
 * - Standard Dijkstra uses a binary heap or Fibonacci heap, incurring an
 *   O(log V) overhead per priority queue operation.
 * - In Dial's algorithm, we observe that shortest path distances increase
 *   monotonically. At any point, the maximum difference between any unvisited
 *   node's tentative distance and the current minimum distance is bounded
 *   by W_max.
 * - Dial uses a circular array of (W_max + 1) buckets (linked lists / vectors).
 * - A vertex with tentative distance d is placed in bucket (d mod (W_max + 1)).
 * - The algorithm advances a bucket index pointer circularly. Since edge
 *   relaxations only move distances forward by at most W_max, vertices are
 *   never inserted behind the current pointer.
 * 
 * Time Complexity:
 * - O(E + V * W_max)
 * - For small W_max (e.g., small constant weight graphs, game grids), this
 *   achieves linear O(V + E) runtime, outperforming heap-based Dijkstra.
 * - Space Complexity: O(V + W_max)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <list>
#include <limits>
#include <cstdint>
#include <iomanip>

namespace dial {

constexpr int INF = std::numeric_limits<int>::max() / 2;

struct Edge {
    int to;
    int weight;
};

class Graph {
public:
    int n;
    int max_weight;
    std::vector<std::vector<Edge>> adj;

    Graph(int vertices, int max_w) : n(vertices), max_weight(max_w), adj(vertices) {}

    void addEdge(int u, int v, int w) {
        adj[u].push_back({v, w});
    }

    /**
     * Executes Dial's algorithm using (max_weight + 1) circular buckets.
     */
    std::vector<int> shortestPath(int source) const {
        std::vector<int> dist(n, INF);
        dist[source] = 0;

        int num_buckets = max_weight + 1;
        std::vector<std::list<int>> buckets(num_buckets);

        // Track list iterator position for O(1) removal on relaxation
        std::vector<std::list<int>::iterator> pos(n);
        std::vector<bool> in_bucket(n, false);

        buckets[0].push_back(source);
        pos[source] = --buckets[0].end();
        in_bucket[source] = true;

        int current_dist = 0;
        int remaining_nodes = 1;

        while (remaining_nodes > 0) {
            // Find the next non-empty bucket
            while (buckets[current_dist % num_buckets].empty()) {
                current_dist++;
            }

            int bucket_idx = current_dist % num_buckets;
            int u = buckets[bucket_idx].front();
            buckets[bucket_idx].pop_front();
            in_bucket[u] = false;
            remaining_nodes--;

            // If the pulled node was already relaxed with a better distance, skip
            if (dist[u] < current_dist) {
                continue;
            }

            for (const auto& edge : adj[u]) {
                int v = edge.to;
                int w = edge.weight;

                if (dist[u] + w < dist[v]) {
                    if (in_bucket[v]) {
                        int old_bucket = dist[v] % num_buckets;
                        buckets[old_bucket].erase(pos[v]);
                        remaining_nodes--;
                    }

                    dist[v] = dist[u] + w;
                    int new_bucket = dist[v] % num_buckets;
                    buckets[new_bucket].push_back(v);
                    pos[v] = --buckets[new_bucket].end();
                    in_bucket[v] = true;
                    remaining_nodes++;
                }
            }
        }

        return dist;
    }
};

} // namespace dial

int main() {
    std::cout << "========================================================\n";
    std::cout << " Dial's Algorithm (Circular Bucket Dijkstra)\n";
    std::cout << "========================================================\n\n";

    int n = 7;
    int max_w = 5;
    dial::Graph g(n, max_w);

    // Build sample graph with edge weights in [1, 5]
    g.addEdge(0, 1, 3);
    g.addEdge(0, 2, 1);
    g.addEdge(1, 2, 2);
    g.addEdge(1, 3, 4);
    g.addEdge(2, 3, 1);
    g.addEdge(2, 4, 5);
    g.addEdge(3, 4, 2);
    g.addEdge(3, 5, 3);
    g.addEdge(4, 5, 1);
    g.addEdge(4, 6, 4);
    g.addEdge(5, 6, 2);

    int source = 0;
    std::cout << "Solving shortest paths from source " << source << " on graph with W_max = " << max_w << "...\n\n";

    auto distances = g.shortestPath(source);

    std::cout << "Shortest Distances using Dial's Bucket Optimization:\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << std::setw(10) << "Vertex" << std::setw(18) << "Distance" << "\n";
    std::cout << "--------------------------------------------------------\n";

    for (int i = 0; i < n; ++i) {
        std::cout << std::setw(10) << i;
        if (distances[i] >= dial::INF) {
            std::cout << std::setw(18) << "UNREACHABLE\n";
        } else {
            std::cout << std::setw(18) << distances[i] << "\n";
        }
    }

    std::cout << "--------------------------------------------------------\n";
    std::cout << "Dial's algorithm completed with O(E + V * W_max) efficiency!\n";
    return 0;
}
