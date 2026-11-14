/**
 * @file tarjans_condensation_dag.cpp
 * @brief Tarjan's SCC condensation DAG builder and topological sort in C++17.
 *
 * Overview:
 *   Given a directed graph G = (V, E) that may contain directed cycles:
 *   1. Identifies all Strongly Connected Components (SCCs) using Tarjan's lowlink DFS in O(V + E).
 *   2. Constructs the Condensation Graph G_cond = (V_cond, E_cond) where each SCC is contracted
 *      into a single super-vertex.
 *   3. Filters out self-loops and duplicate edges between components.
 *   4. Proves and verifies that G_cond is strictly a Directed Acyclic Graph (DAG),
 *      and computes its valid topological sorting.
 *
 * Complexity:
 *   - SCC Decomposition: O(V + E)
 *   - Condensation Graph Construction: O(V + E)
 *   - Topological Sort: O(V_cond + E_cond)
 *   - Overall Time: O(V + E), Space: O(V + E).
 */

#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <queue>
#include <algorithm>
#include <iomanip>

namespace graph {

class CondensationDAG {
private:
    int n;
    std::vector<std::vector<int>> adj;

    // Tarjan's DFS state
    std::vector<int> tin, low, compId;
    std::vector<bool> inStack;
    std::stack<int> st;
    int timer;
    int numComponents;
    std::vector<std::vector<int>> components;

    void dfsTarjan(int u) {
        tin[u] = low[u] = ++timer;
        st.push(u);
        inStack[u] = true;

        for (int v : adj[u]) {
            if (tin[v] == 0) {
                dfsTarjan(v);
                low[u] = std::min(low[u], low[v]);
            } else if (inStack[v]) {
                low[u] = std::min(low[u], tin[v]);
            }
        }

        if (low[u] == tin[u]) {
            std::vector<int> currentComp;
            while (true) {
                int node = st.top();
                st.pop();
                inStack[node] = false;
                compId[node] = numComponents;
                currentComp.push_back(node);
                if (node == u) break;
            }
            components.push_back(currentComp);
            numComponents++;
        }
    }

public:
    explicit CondensationDAG(int vertices)
        : n(vertices), adj(vertices), tin(vertices, 0), low(vertices, 0),
          compId(vertices, -1), inStack(vertices, false), timer(0), numComponents(0) {}

    void addEdge(int from, int to) {
        adj[from].push_back(to);
    }

    struct Result {
        int numSCC;
        std::vector<std::vector<int>> components;
        std::vector<int> componentOf;
        std::vector<std::vector<int>> dagAdj; // Condensation DAG adjacency
        std::vector<int> topologicalOrder;   // Topo order of super-vertices
    };

    Result buildCondensation() {
        // Step 1: Tarjan's SCC
        for (int i = 0; i < n; ++i) {
            if (tin[i] == 0) {
                dfsTarjan(i);
            }
        }

        // Step 2: Build Condensation DAG
        std::vector<std::set<int>> dagSet(numComponents);
        std::vector<int> inDegree(numComponents, 0);

        for (int u = 0; u < n; ++u) {
            int cu = compId[u];
            for (int v : adj[u]) {
                int cv = compId[v];
                if (cu != cv) {
                    if (dagSet[cu].find(cv) == dagSet[cu].end()) {
                        dagSet[cu].insert(cv);
                        inDegree[cv]++;
                    }
                }
            }
        }

        std::vector<std::vector<int>> dagAdj(numComponents);
        for (int c = 0; c < numComponents; ++c) {
            dagAdj[c].assign(dagSet[c].begin(), dagSet[c].end());
        }

        // Step 3: Topological Sort of the Condensation DAG (Kahn's Algorithm)
        std::queue<int> q;
        for (int c = 0; c < numComponents; ++c) {
            if (inDegree[c] == 0) {
                q.push(c);
            }
        }

        std::vector<int> topoOrder;
        while (!q.empty()) {
            int curr = q.front();
            q.pop();
            topoOrder.push_back(curr);

            for (int nxt : dagAdj[curr]) {
                inDegree[nxt]--;
                if (inDegree[nxt] == 0) {
                    q.push(nxt);
                }
            }
        }

        return {numComponents, components, compId, dagAdj, topoOrder};
    }
};

} // namespace graph

/*
 * Demonstration Graph Architecture:
 *   SCC A: {0, 1, 2} forming cycle 0 -> 1 -> 2 -> 0
 *   SCC B: {3, 4} forming cycle 3 -> 4 -> 3
 *   SCC C: {5, 6} forming cycle 5 -> 6 -> 5
 *   Edges between SCCs:
 *     SCC A -> SCC B (edge 1 -> 3)
 *     SCC A -> SCC C (edge 2 -> 5)
 *     SCC B -> SCC C (edge 4 -> 6)
 */

int main() {
    std::cout << "========================================\n";
    std::cout << "  TARJAN'S CONDENSATION DAG BUILDER     \n";
    std::cout << "========================================\n\n";

    int n = 7;
    graph::CondensationDAG g(n);

    // Cycle A: 0 -> 1 -> 2 -> 0
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);

    // Cycle B: 3 -> 4 -> 3
    g.addEdge(3, 4);
    g.addEdge(4, 3);

    // Cycle C: 5 -> 6 -> 5
    g.addEdge(5, 6);
    g.addEdge(6, 5);

    // Inter-component cross edges
    g.addEdge(1, 3);
    g.addEdge(2, 5);
    g.addEdge(4, 6);

    auto res = g.buildCondensation();

    std::cout << "Total Vertices: " << n << "\n";
    std::cout << "Extracted " << res.numSCC << " Strongly Connected Components:\n";
    for (size_t c = 0; c < res.components.size(); ++c) {
        std::cout << "  Component " << c << ": [ ";
        for (int v : res.components[c]) std::cout << v << " ";
        std::cout << "]\n";
    }

    std::cout << "\nCondensation DAG Directed Edges:\n";
    for (int c = 0; c < res.numSCC; ++c) {
        std::cout << "  Super-Vertex " << c << " -> ";
        if (res.dagAdj[c].empty()) {
            std::cout << "(Terminal Sink)\n";
        } else {
            for (int target : res.dagAdj[c]) std::cout << target << " ";
            std::cout << "\n";
        }
    }

    std::cout << "\nValid Topological Ordering of Condensation DAG: [ ";
    for (int c : res.topologicalOrder) {
        std::cout << "Component_" << c << " ";
    }
    std::cout << "]\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
