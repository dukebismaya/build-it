/*
 * Program Description:
 * Tarjan's Algorithm for Finding Articulation Points (Cut Vertices) in Undirected Graphs.
 * A vertex is an Articulation Point if removing it (along with its incident edges)
 * increases the number of connected components in the graph.
 *
 * Algorithm Details (DFS Tree):
 * Maintains:
 * 1. tin[u]: discovery time of vertex u.
 * 2. low[u]: lowest discovery time reachable from u through at most one back-edge.
 *
 * Conditions for vertex u to be an articulation point:
 * 1. If u is the ROOT of DFS tree: u is an articulation point iff it has >= 2 children in DFS tree.
 * 2. If u is NOT the root: u is an articulation point iff it has a child v such that low[v] >= tin[u].
 *
 * Time Complexity: O(V + E)
 * Space Complexity: O(V + E)
 */

#include <iostream>
#include <vector>
#include <set>

class GraphArticulationPoints {
    int numVertices;
    std::vector<std::vector<int>> adj;
    std::vector<int> tin, low;
    std::vector<bool> visited;
    std::set<int> articulationPoints;
    int timer = 0;

    void dfs(int u, int p = -1) {
        visited[u] = true;
        tin[u] = low[u] = ++timer;
        int children = 0;

        for (int v : adj[u]) {
            if (v == p) continue; // Skip edge leading back to parent

            if (visited[v]) {
                // Back-edge found
                low[u] = std::min(low[u], tin[v]);
            } else {
                // Forward edge in DFS tree
                dfs(v, u);
                low[u] = std::min(low[u], low[v]);

                if (low[v] >= tin[u] && p != -1) {
                    articulationPoints.insert(u);
                }
                children++;
            }
        }

        // Special case: root node
        if (p == -1 && children > 1) {
            articulationPoints.insert(u);
        }
    }

public:
    explicit GraphArticulationPoints(int v)
        : numVertices(v), adj(v), tin(v, -1), low(v, -1), visited(v, false) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    std::set<int> findArticulationPoints() {
        articulationPoints.clear();
        tin.assign(numVertices, -1);
        low.assign(numVertices, -1);
        visited.assign(numVertices, false);
        timer = 0;

        for (int i = 0; i < numVertices; ++i) {
            if (!visited[i]) {
                dfs(i);
            }
        }

        return articulationPoints;
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "  Tarjan's Articulation Points (O(V + E)) \n";
    std::cout << "==========================================\n\n";

    // Graph 1:
    // 0 - 1 - 2
    // |   |
    // 3 - 4
    // Vertex 1 is an articulation point because removing it disconnects 2 from the rest.
    GraphArticulationPoints g1(5);
    g1.addEdge(0, 1);
    g1.addEdge(1, 2);
    g1.addEdge(1, 4);
    g1.addEdge(4, 3);
    g1.addEdge(3, 0);

    auto ap1 = g1.findArticulationPoints();
    std::cout << "Graph 1 Articulation Points: ";
    for (int v : ap1) std::cout << v << " ";
    std::cout << " (Expected: 1)\n";

    // Graph 2: Classic bridge & cut vertex graph
    // 0 - 1 - 2 - 3 - 4
    //     |   |
    //     +---+
    // Vertices 1 and 2 form a cycle; vertices 0, 1, 2, 3 are articulation points.
    GraphArticulationPoints g2(5);
    g2.addEdge(0, 1);
    g2.addEdge(1, 2);
    g2.addEdge(2, 3);
    g2.addEdge(3, 4);

    auto ap2 = g2.findArticulationPoints();
    std::cout << "Graph 2 (Linear Path 0-1-2-3-4) Articulation Points: ";
    for (int v : ap2) std::cout << v << " ";
    std::cout << " (Expected: 1 2 3)\n";

    return 0;
}
