/**
 * @file gabow_strongly_connected_components.cpp
 * @brief Cheriyan-Mehlhorn / Gabow's path-based SCC algorithm in C++17.
 *
 * Overview (Harold N. Gabow, 2000; Cheriyan & Mehlhorn, 1996):
 *   A linear-time algorithm to decompose a directed graph into its Strongly
 *   Connected Components (SCCs).
 *
 * Comparison to Tarjan's & Kosaraju's Algorithms:
 *   - Kosaraju requires two full DFS passes (one on G, one on the transpose G^T).
 *   - Tarjan requires tracking integer discovery times and minimum lowlinks.
 *   - Gabow uses a single DFS pass with two stacks (S and B) to track the path
 *     boundaries, contracting cycles along the current DFS tree without computing
 *     lowlink values.
 *
 * Invariant:
 *   - Stack S contains all vertices visited that are not yet assigned to an SCC.
 *   - Stack B contains the root boundaries of candidate SCCs on the current DFS path.
 *
 * Complexity:
 *   - Time: O(V + E) single DFS traversal.
 *   - Space: O(V) for the two stacks and component tracking.
 */

#include <iostream>
#include <vector>
#include <stack>
#include <iomanip>

namespace graph {

class GabowSCC {
private:
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<int> preorder;
    std::vector<int> compId;
    std::stack<int> S; // All visited vertices not yet contracted
    std::stack<int> B; // Boundary vertices of candidate components
    int timer;
    int numComponents;
    std::vector<std::vector<int>> components;

    void dfs(int u) {
        preorder[u] = ++timer;
        S.push(u);
        B.push(u);

        for (int v : adj[u]) {
            if (preorder[v] == 0) {
                dfs(v);
            } else if (compId[v] == -1) {
                // v is in current DFS path but not in any completed SCC
                // Contract cycle by popping boundaries whose preorder > preorder[v]
                while (!B.empty() && preorder[B.top()] > preorder[v]) {
                    B.pop();
                }
            }
        }

        // If u is the boundary root of an SCC
        if (!B.empty() && B.top() == u) {
            B.pop();
            std::vector<int> currentComp;
            while (true) {
                int node = S.top();
                S.pop();
                compId[node] = numComponents;
                currentComp.push_back(node);
                if (node == u) break;
            }
            components.push_back(currentComp);
            numComponents++;
        }
    }

public:
    explicit GabowSCC(int vertices)
        : n(vertices), adj(vertices), preorder(vertices, 0),
          compId(vertices, -1), timer(0), numComponents(0) {}

    void addEdge(int from, int to) {
        adj[from].push_back(to);
    }

    void compute() {
        for (int i = 0; i < n; ++i) {
            if (preorder[i] == 0) {
                dfs(i);
            }
        }
    }

    const std::vector<std::vector<int>>& getComponents() const {
        return components;
    }

    int getNumComponents() const {
        return numComponents;
    }
};

} // namespace graph

/*
 * Demonstration Directed Graph:
 *   SCC 1: {0, 1, 2} forming cycle 0 -> 1 -> 2 -> 0
 *   Bridge: 2 -> 3
 *   SCC 2: {3, 4} forming cycle 3 -> 4 -> 3
 *   Bridge: 4 -> 5
 *   SCC 3: {5} (singleton)
 */

int main() {
    std::cout << "========================================\n";
    std::cout << "    GABOW'S PATH-BASED SCC ALGORITHM    \n";
    std::cout << "        (Two-Stack O(V + E) DFS)        \n";
    std::cout << "========================================\n\n";

    int vertices = 6;
    graph::GabowSCC g(vertices);

    // Cycle 0 -> 1 -> 2 -> 0
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);

    // Connection from SCC 1 to SCC 2
    g.addEdge(2, 3);

    // Cycle 3 -> 4 -> 3
    g.addEdge(3, 4);
    g.addEdge(4, 3);

    // Connection from SCC 2 to SCC 3
    g.addEdge(4, 5);

    g.compute();

    auto sccs = g.getComponents();
    std::cout << "Total Strongly Connected Components: " << g.getNumComponents() << "\n\n";

    for (size_t i = 0; i < sccs.size(); ++i) {
        std::cout << "Component " << (i + 1) << " (" << sccs[i].size() << " vertices): [ ";
        for (int v : sccs[i]) {
            std::cout << v << " ";
        }
        std::cout << "]\n";
    }

    std::cout << "\nProgram finished successfully.\n";
    return 0;
}
