/**
 * @file topological_sort_all_orders.cpp
 * @brief Backtracking generator for ALL valid topological orderings of a DAG in C++17.
 *
 * A topological sort of a Directed Acyclic Graph (DAG) is a linear ordering of its
 * vertices such that for every directed edge (u, v), vertex u comes before v.
 * While Kahn's algorithm or DFS finds one valid topological sort in O(V + E),
 * a DAG can admit exponentially many valid topological orderings.
 *
 * Algorithm:
 *   - Maintain an in-degree array and a visited flag for each vertex.
 *   - At each recursion level, find all currently unvisited vertices with in-degree 0.
 *   - For each candidate vertex:
 *       1. Add it to the current path and mark as visited.
 *       2. Decrement in-degrees of all its adjacent neighbors.
 *       3. Recurse.
 *       4. Backtrack: restore in-degrees, unmark visited, remove from path.
 *   - When path length reaches |V|, record the valid ordering.
 *
 * Complexity:
 *   - Time: O(K * V) where K is the total number of topological orderings.
 *   - Space: O(V + E) recursion depth and graph storage.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

namespace graph {

class AllTopologicalSorts {
private:
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<int> inDegree;
    std::vector<bool> visited;
    std::vector<int> currentPath;
    std::vector<std::vector<int>> allOrders;

    void backtrack() {
        bool flag = false;

        for (int i = 0; i < n; ++i) {
            if (!visited[i] && inDegree[i] == 0) {
                // Choose vertex i
                visited[i] = true;
                currentPath.push_back(i);

                for (int next : adj[i]) {
                    inDegree[next]--;
                }

                backtrack();

                // Undo choice (backtrack)
                for (int next : adj[i]) {
                    inDegree[next]++;
                }
                currentPath.pop_back();
                visited[i] = false;

                flag = true;
            }
        }

        // Base case: all vertices placed
        if (!flag && currentPath.size() == static_cast<size_t>(n)) {
            allOrders.push_back(currentPath);
        }
    }

public:
    explicit AllTopologicalSorts(int vertices)
        : n(vertices), adj(vertices), inDegree(vertices, 0), visited(vertices, false) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        inDegree[v]++;
    }

    std::vector<std::vector<int>> findAllSorts() {
        allOrders.clear();
        currentPath.clear();
        std::fill(visited.begin(), visited.end(), false);
        backtrack();
        return allOrders;
    }
};

} // namespace graph

/*
 * Demonstration Graph:
 *
 *      (5) ------> (0) <------ (4)
 *       |                       |
 *       v                       v
 *      (2) ------> (3) ------> (1)
 *
 * Edges:
 *   5 -> 2, 5 -> 0
 *   4 -> 0, 4 -> 1
 *   2 -> 3
 *   3 -> 1
 */

int main() {
    std::cout << "========================================\n";
    std::cout << "  ALL TOPOLOGICAL SORT ORDERINGS (DAG)  \n";
    std::cout << "========================================\n\n";

    int vertices = 6;
    graph::AllTopologicalSorts dag(vertices);

    dag.addEdge(5, 2);
    dag.addEdge(5, 0);
    dag.addEdge(4, 0);
    dag.addEdge(4, 1);
    dag.addEdge(2, 3);
    dag.addEdge(3, 1);

    auto orders = dag.findAllSorts();

    std::cout << "Graph with " << vertices << " vertices.\n";
    std::cout << "Edges: 5->2, 5->0, 4->0, 4->1, 2->3, 3->1\n";
    std::cout << "Total Valid Topological Sorts Found: " << orders.size() << "\n\n";

    for (size_t i = 0; i < orders.size(); ++i) {
        std::cout << "Ordering " << std::setw(2) << (i + 1) << ": [ ";
        for (size_t j = 0; j < orders[i].size(); ++j) {
            std::cout << orders[i][j] << (j + 1 == orders[i].size() ? "" : ", ");
        }
        std::cout << " ]\n";
    }

    std::cout << "\nProgram finished successfully.\n";
    return 0;
}
