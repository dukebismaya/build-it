/*
 * Program Description:
 * 0-1 Breadth-First Search (0-1 BFS) Algorithm.
 * Finds the shortest path in a graph where all edge weights are strictly 0 or 1.
 *
 * Algorithm Insight:
 * Dijkstra's algorithm uses a priority queue and takes O((V + E) log V).
 * In 0-1 BFS, we use a double-ended queue (std::deque):
 * - If relaxation uses an edge of weight 0, push to FRONT of deque.
 * - If relaxation uses an edge of weight 1, push to BACK of deque.
 * This guarantees the deque elements remain monotonically ordered by distance,
 * achieving true linear time O(V + E).
 */

#include <iostream>
#include <vector>
#include <deque>
#include <climits>

struct Edge {
    int to;
    int weight; // 0 or 1
};

class Graph01BFS {
    int numVertices;
    std::vector<std::vector<Edge>> adj;

public:
    explicit Graph01BFS(int v) : numVertices(v), adj(v) {}

    void addEdge(int u, int v, int weight) {
        adj[u].push_back({v, weight});
    }

    // Computes shortest distances from source in O(V + E)
    std::vector<int> shortestPath(int src) {
        std::vector<int> dist(numVertices, INT_MAX);
        std::deque<int> dq;

        dist[src] = 0;
        dq.push_back(src);

        while (!dq.empty()) {
            int u = dq.front();
            dq.pop_front();

            for (const auto& edge : adj[u]) {
                int v = edge.to;
                int w = edge.weight;

                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;

                    if (w == 0) {
                        dq.push_front(v);
                    } else {
                        dq.push_back(v);
                    }
                }
            }
        }

        return dist;
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "           0-1 BFS Shortest Path          \n";
    std::cout << "==========================================\n\n";

    // Graph with 6 vertices (0 to 5)
    // Edge weights represent e.g. Free highway (0) vs Toll road (1)
    Graph01BFS graph(6);
    graph.addEdge(0, 1, 0);
    graph.addEdge(0, 2, 1);
    graph.addEdge(1, 3, 1);
    graph.addEdge(2, 3, 0);
    graph.addEdge(3, 4, 1);
    graph.addEdge(2, 4, 1);
    graph.addEdge(4, 5, 0);

    int source = 0;
    std::cout << "Finding shortest distance from source vertex " << source << ":\n\n";

    std::vector<int> dist = graph.shortestPath(source);

    for (int i = 0; i < 6; ++i) {
        std::cout << "Distance to vertex " << i << ": ";
        if (dist[i] == INT_MAX) {
            std::cout << "Unreachable\n";
        } else {
            std::cout << dist[i] << "\n";
        }
    }

    std::cout << "\nComparison Note:\n";
    std::cout << "- 0-1 BFS Time Complexity: O(V + E) using std::deque\n";
    std::cout << "- Standard Dijkstra Complexity: O((V + E) log V)\n";
    std::cout << "0-1 BFS provides optimal linear performance on binary-cost networks.\n";

    return 0;
}
