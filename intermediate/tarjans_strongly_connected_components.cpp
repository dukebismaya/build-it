/*
 * Problem Description:
 * Tarjan's Strongly Connected Components (SCC) Algorithm.
 * Given a directed graph G = (V, E), find all strongly connected components (maximal subgraphs
 * where every vertex is reachable from every other vertex within the subgraph).
 * Tarjan's algorithm uses a single Depth First Search traversal and maintains:
 * - discovery time (tin) for each vertex
 * - low-link value (low) representing the lowest discovery time reachable via DFS tree or back-edges
 * - an active traversal stack to track vertices currently in the SCC search space.
 * Time Complexity: O(V + E)
 * Space Complexity: O(V)
 */

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

class TarjanSCC {
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<int> tin, low;
    std::vector<bool> onStack;
    std::stack<int> st;
    int timer;
    std::vector<std::vector<int>> sccs;

    void dfs(int u) {
        tin[u] = low[u] = ++timer;
        st.push(u);
        onStack[u] = true;

        for (int v : adj[u]) {
            if (tin[v] == -1) {
                dfs(v);
                low[u] = std::min(low[u], low[v]);
            } else if (onStack[v]) {
                low[u] = std::min(low[u], tin[v]);
            }
        }

        if (low[u] == tin[u]) {
            std::vector<int> component;
            while (true) {
                int node = st.top();
                st.pop();
                onStack[node] = false;
                component.push_back(node);
                if (node == u) break;
            }
            std::sort(component.begin(), component.end());
            sccs.push_back(component);
        }
    }

public:
    TarjanSCC(int vertices) : n(vertices), adj(vertices), tin(vertices, -1),
                              low(vertices, -1), onStack(vertices, false), timer(0) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
    }

    std::vector<std::vector<int>> getSCCs() {
        for (int i = 0; i < n; ++i) {
            if (tin[i] == -1) {
                dfs(i);
            }
        }
        return sccs;
    }
};

int main() {
    int vertices = 8;
    TarjanSCC graph(vertices);

    // Graph edges
    graph.addEdge(0, 1);
    graph.addEdge(1, 2);
    graph.addEdge(2, 0);
    graph.addEdge(2, 3);
    graph.addEdge(3, 4);
    graph.addEdge(4, 5);
    graph.addEdge(5, 6);
    graph.addEdge(6, 4);
    graph.addEdge(4, 7);

    std::vector<std::vector<int>> sccs = graph.getSCCs();

    std::cout << "Strongly Connected Components (Tarjan's Algorithm):\n";
    for (size_t i = 0; i < sccs.size(); ++i) {
        std::cout << "SCC #" << (i + 1) << ": { ";
        for (int node : sccs[i]) {
            std::cout << node << " ";
        }
        std::cout << "}\n";
    }

    return 0;
}
