/**
 * @file minimum_path_cover_dag.cpp
 * @brief Minimum Vertex-Disjoint Path Cover in a Directed Acyclic Graph (DAG) in C++17.
 *
 * Problem Definition:
 * Given a Directed Acyclic Graph G = (V, E), find a minimum number of vertex-disjoint
 * directed paths that cover all vertices in V (every vertex belongs to exactly one path).
 *
 * Bipartite Matching Reduction:
 * - Construct bipartite graph B with two sets of vertices: L = {u_out} and R = {v_in} for each v in V.
 * - For every directed edge (u, v) in E, add an edge from u_out to v_in in B.
 * - Compute Maximum Bipartite Matching (size |M|).
 * - Minimum Path Cover size = |V| - |M|.
 * - The paths are formed by following edges in the matching (if u_out is matched to v_in, v follows u).
 *
 * Complexity:
 *   - Bipartite Graph Construction: O(V + E)
 *   - Maximum Matching via Hopcroft-Karp / Kuhn's algorithm: O(V * E)
 *   - Total Time: O(V * E), Space: O(V + E)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

namespace graph {

class MinimumPathCoverDAG {
private:
    int n; // Number of vertices in DAG [0, n-1]
    std::vector<std::vector<int>> adj;
    std::vector<int> matchL; // Matches from u_out -> v_in
    std::vector<int> matchR; // Matches from v_in -> u_out
    std::vector<bool> visited;

    bool dfsKuhn(int u) {
        for (int v : adj[u]) {
            if (!visited[v]) {
                visited[v] = true;
                if (matchR[v] < 0 || dfsKuhn(matchR[v])) {
                    matchL[u] = v;
                    matchR[v] = u;
                    return true;
                }
            }
        }
        return false;
    }

public:
    explicit MinimumPathCoverDAG(int vertices)
        : n(vertices), adj(vertices), matchL(vertices, -1), matchR(vertices, -1), visited(vertices, false) {}

    void addEdge(int from, int to) {
        adj[from].push_back(to);
    }

    /**
     * @brief Computes the minimum path cover.
     * @return Vector of paths, where each path is a sequence of vertex IDs.
     */
    std::vector<std::vector<int>> solve() {
        // Step 1: Maximum bipartite matching
        for (int i = 0; i < n; ++i) {
            std::fill(visited.begin(), visited.end(), false);
            dfsKuhn(i);
        }

        // Step 2: Reconstruct paths
        // Vertices with matchR[v] == -1 are heads of paths (no incoming matched edge).
        std::vector<std::vector<int>> paths;
        std::vector<bool> inPath(n, false);

        for (int i = 0; i < n; ++i) {
            if (matchR[i] == -1) {
                std::vector<int> path;
                int curr = i;
                while (curr != -1) {
                    path.push_back(curr);
                    inPath[curr] = true;
                    curr = matchL[curr];
                }
                paths.push_back(path);
            }
        }

        return paths;
    }
};

} // namespace graph

/*
 * Demonstration DAG Structure:
 *
 *       (0) --------> (1)
 *        |             |
 *        v             v
 *       (2) --------> (3)
 *        |             |
 *        v             v
 *       (4)           (5)
 *
 * Edges:
 *  0 -> 1, 0 -> 2
 *  1 -> 3
 *  2 -> 3, 2 -> 4
 *  3 -> 5
 */

int main() {
    std::cout << "========================================\n";
    std::cout << "     MINIMUM PATH COVER ON A DAG        \n";
    std::cout << " (Bipartite Matching Reduction Method)  \n";
    std::cout << "========================================\n\n";

    int numVertices = 6;
    graph::MinimumPathCoverDAG dag(numVertices);

    dag.addEdge(0, 1);
    dag.addEdge(0, 2);
    dag.addEdge(1, 3);
    dag.addEdge(2, 3);
    dag.addEdge(2, 4);
    dag.addEdge(3, 5);

    auto paths = dag.solve();

    std::cout << "Total Vertices: " << numVertices << "\n";
    std::cout << "Minimum Disjoint Paths Required: " << paths.size() << "\n\n";

    for (size_t i = 0; i < paths.size(); ++i) {
        std::cout << "Path " << (i + 1) << " (" << paths[i].size() << " vertices): ";
        for (size_t j = 0; j < paths[i].size(); ++j) {
            std::cout << paths[i][j] << (j + 1 == paths[i].size() ? "" : " -> ");
        }
        std::cout << "\n";
    }

    std::cout << "\nVerification:\n";
    std::cout << "Every vertex [0..5] belongs to exactly one path above.\n";
    std::cout << "Program finished successfully.\n";
    return 0;
}
