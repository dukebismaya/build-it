/**
 * @file bipartite_graph_checker_bfs.cpp
 * @brief Checks if an undirected graph is bipartite using Breadth-First Search (2-coloring).
 *
 * Problem:
 * A graph is bipartite if its vertices can be divided into two independent sets U and V
 * such that every edge connects a vertex in U to a vertex in V (no edge connects vertices
 * in the same set). Equivalently, a graph is bipartite if and only if it contains no odd cycles.
 *
 * Algorithm:
 * - Initialize an array `color` of size V with -1 (unvisited).
 * - For each unvisited vertex, start BFS and assign color 0.
 * - For each neighbor of current vertex:
 *   - If neighbor is uncolored, assign color `1 - currentColor` and push to queue.
 *   - If neighbor is already colored with `currentColor`, a conflict (odd cycle) is detected -> Not Bipartite.
 * - Handles disconnected components seamlessly.
 * - If bipartite, returns the two disjoint vertex sets.
 *
 * Time Complexity: O(V + E)
 * Space Complexity: O(V)
 */

#include <iostream>
#include <vector>
#include <queue>
#include <iomanip>

struct BipartiteResult {
    bool isBipartite;
    std::vector<int> colors; // 0 or 1 for each vertex, -1 if isolated/unvisited
    std::vector<int> setA;
    std::vector<int> setB;
    int conflictU = -1;
    int conflictV = -1;
};

class Graph {
private:
    int numVertices;
    std::vector<std::vector<int>> adj;

public:
    explicit Graph(int vertices) : numVertices(vertices), adj(vertices) {}

    void addEdge(int u, int v) {
        if (u >= 0 && u < numVertices && v >= 0 && v < numVertices) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }

    int getVertexCount() const { return numVertices; }
    const std::vector<int>& getNeighbors(int u) const { return adj[u]; }

    BipartiteResult checkBipartite() const {
        BipartiteResult result;
        result.isBipartite = true;
        result.colors.assign(numVertices, -1);

        for (int start = 0; start < numVertices; ++start) {
            if (result.colors[start] != -1) continue;

            std::queue<int> q;
            result.colors[start] = 0;
            q.push(start);

            while (!q.empty()) {
                int u = q.front();
                q.pop();

                for (int v : adj[u]) {
                    if (result.colors[v] == -1) {
                        result.colors[v] = 1 - result.colors[u];
                        q.push(v);
                    } else if (result.colors[v] == result.colors[u]) {
                        result.isBipartite = false;
                        result.conflictU = u;
                        result.conflictV = v;
                        return result; // Early termination on odd-cycle detection
                    }
                }
            }
        }

        for (int i = 0; i < numVertices; ++i) {
            if (result.colors[i] == 0) {
                result.setA.push_back(i);
            } else if (result.colors[i] == 1) {
                result.setB.push_back(i);
            }
        }

        return result;
    }
};

void runDemo(const std::string& name, const Graph& g) {
    std::cout << "--- " << name << " (" << g.getVertexCount() << " vertices) ---\n";
    auto res = g.checkBipartite();

    if (res.isBipartite) {
        std::cout << "Result: GRAPH IS BIPARTITE (2-Colorable)\n";
        std::cout << "  Partition Set A (Color 0): { ";
        for (int v : res.setA) std::cout << v << " ";
        std::cout << "}\n";
        std::cout << "  Partition Set B (Color 1): { ";
        for (int v : res.setB) std::cout << v << " ";
        std::cout << "}\n";
    } else {
        std::cout << "Result: GRAPH IS NOT BIPARTITE (Contains Odd-Length Cycle)\n";
        std::cout << "  Conflict detected between vertices " << res.conflictU 
                  << " and " << res.conflictV << " (both share the same color)\n";
    }
    std::cout << '\n';
}

int main() {
    std::cout << "=== Bipartite Graph Verification (BFS 2-Coloring) ===\n\n";

    // Example 1: Even cycle C4 (Bipartite)
    Graph g1(4);
    g1.addEdge(0, 1);
    g1.addEdge(1, 2);
    g1.addEdge(2, 3);
    g1.addEdge(3, 0);
    runDemo("Graph 1: 4-Cycle C4", g1);

    // Example 2: Triangle C3 (Odd cycle -> Not Bipartite)
    Graph g2(3);
    g2.addEdge(0, 1);
    g2.addEdge(1, 2);
    g2.addEdge(2, 0);
    runDemo("Graph 2: Triangle C3", g2);

    // Example 3: Disconnected components (both bipartite)
    Graph g3(6);
    g3.addEdge(0, 1);
    g3.addEdge(1, 2);
    g3.addEdge(3, 4);
    g3.addEdge(4, 5);
    runDemo("Graph 3: Disconnected Bipartite Components", g3);

    // Example 4: Complete Bipartite Graph K(2, 3)
    Graph g4(5);
    // Left: 0, 1; Right: 2, 3, 4
    g4.addEdge(0, 2); g4.addEdge(0, 3); g4.addEdge(0, 4);
    g4.addEdge(1, 2); g4.addEdge(1, 3); g4.addEdge(1, 4);
    runDemo("Graph 4: Complete Bipartite K(2, 3)", g4);

    return 0;
}
