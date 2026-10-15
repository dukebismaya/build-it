/*
 * Program Description:
 * Kahn's Algorithm for Topological Sorting of Directed Graphs.
 * Topological sort produces a linear ordering of vertices such that for every directed
 * edge u -> v, vertex u comes before v.
 *
 * Algorithm Details:
 * 1. Compute in-degree for each vertex.
 * 2. Enqueue all vertices with in-degree 0.
 * 3. While queue is not empty, dequeue u, append to topological order,
 *    and reduce in-degree of all adjacent vertices v. If in-degree(v) becomes 0, enqueue v.
 * 4. Cycle Detection: If the number of processed vertices != total vertices,
 *    the graph contains a cycle (cannot be topologically sorted).
 *
 * Time Complexity: O(V + E)
 * Space Complexity: O(V + E)
 */

#include <iostream>
#include <vector>
#include <queue>
#include <string>

class DirectedGraph {
    int numVertices;
    std::vector<std::vector<int>> adj;

public:
    explicit DirectedGraph(int v) : numVertices(v), adj(v) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
    }

    // Runs Kahn's algorithm, returning pair <hasCycle, topologicalOrder>
    std::pair<bool, std::vector<int>> topologicalSortKahn() const {
        std::vector<int> inDegree(numVertices, 0);

        for (int u = 0; u < numVertices; ++u) {
            for (int v : adj[u]) {
                inDegree[v]++;
            }
        }

        std::queue<int> q;
        for (int i = 0; i < numVertices; ++i) {
            if (inDegree[i] == 0) {
                q.push(i);
            }
        }

        std::vector<int> order;
        order.reserve(numVertices);

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            order.push_back(u);

            for (int v : adj[u]) {
                inDegree[v]--;
                if (inDegree[v] == 0) {
                    q.push(v);
                }
            }
        }

        bool hasCycle = (static_cast<int>(order.size()) != numVertices);
        return {hasCycle, order};
    }
};

int main() {
    std::cout << "=====================================================\n";
    std::cout << "  Kahn's Algorithm (Topological Sort & Cycle Check)  \n";
    std::cout << "=====================================================\n\n";

    // Graph 1: Valid DAG (e.g. course prerequisites)
    // 5: Linear Algebra, 4: Calculus, 0: Data Structures, 2: Algorithms, 3: OS, 1: AI
    DirectedGraph dag(6);
    dag.addEdge(5, 2);
    dag.addEdge(5, 0);
    dag.addEdge(4, 0);
    dag.addEdge(4, 1);
    dag.addEdge(2, 3);
    dag.addEdge(3, 1);

    std::cout << "Graph 1 (DAG with 6 vertices):\n";
    auto [cycle1, order1] = dag.topologicalSortKahn();
    if (cycle1) {
        std::cout << "Cycle detected! Topological sort impossible.\n";
    } else {
        std::cout << "Valid Topological Order: ";
        for (size_t i = 0; i < order1.size(); ++i) {
            std::cout << order1[i] << (i + 1 < order1.size() ? " -> " : "\n");
        }
    }

    std::cout << "\n-----------------------------------------------------\n\n";

    // Graph 2: Graph with cycle
    DirectedGraph cyclicGraph(4);
    cyclicGraph.addEdge(0, 1);
    cyclicGraph.addEdge(1, 2);
    cyclicGraph.addEdge(2, 3);
    cyclicGraph.addEdge(3, 1); // Cycle: 1 -> 2 -> 3 -> 1

    std::cout << "Graph 2 (Graph with cycle 1->2->3->1):\n";
    auto [cycle2, order2] = cyclicGraph.topologicalSortKahn();
    if (cycle2) {
        std::cout << "Cycle detected successfully! Graph is not a DAG.\n";
        std::cout << "Processed vertices before cycle deadlock: ";
        for (int v : order2) std::cout << v << " ";
        std::cout << "\n";
    } else {
        std::cout << "Valid Topological Order exists.\n";
    }

    return 0;
}
