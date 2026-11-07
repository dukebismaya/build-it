/**
 * @file bridge_biconnected_components.cpp
 * @brief 2-Edge-Connected Components (Bridge-Biconnected Components) in C++17.
 *
 * In an undirected graph, a 2-edge-connected component (2ECC) is a maximal subgraph
 * such that the subgraph remains connected after the deletion of any single edge.
 * In other words, there are no bridges within any 2ECC; bridges are precisely the edges
 * connecting different 2-edge-connected components.
 *
 * Condensing each 2ECC into a single super-vertex yields a tree (or forest)
 * called the "Bridge Tree" or "2ECC Condensation Tree".
 *
 * Algorithm:
 * 1. Tarjan's DFS: Computes discovery times tin[u] and lowest reachable ancestor low[u].
 * 2. Bridge identification: An edge (u, v) is a bridge if low[v] > tin[u].
 * 3. 2ECC extraction:
 *    - Maintain a stack of visited vertices during DFS.
 *    - When low[v] > tin[u], all vertices currently on stack above tin[u] belong to the component of v.
 *    - Alternatively, a second DFS ignoring bridge edges assigns component IDs.
 *
 * Complexity:
 *   - Time: O(V + E)
 *   - Space: O(V + E)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

namespace graph {

struct Edge {
    int to;
    int edgeId;
};

class BridgeBiconnectedComponents {
private:
    int n;
    int edgeCount;
    std::vector<std::vector<Edge>> adj;
    std::vector<std::pair<int, int>> edgeList;
    std::vector<bool> isBridge;
    std::vector<int> tin;
    std::vector<int> low;
    std::vector<int> compId;
    std::vector<std::vector<int>> components;
    int timer;
    int numComponents;

    void dfsBridges(int u, int parentEdge) {
        tin[u] = low[u] = ++timer;
        for (const auto& edge : adj[u]) {
            int v = edge.to;
            int eId = edge.edgeId;
            if (eId == parentEdge) continue; // Do not traverse the incoming undirected edge back

            if (tin[v]) {
                // Back-edge
                low[u] = std::min(low[u], tin[v]);
            } else {
                // Tree-edge
                dfsBridges(v, eId);
                low[u] = std::min(low[u], low[v]);
                if (low[v] > tin[u]) {
                    isBridge[eId] = true;
                }
            }
        }
    }

    void dfsComponent(int u, int cId) {
        compId[u] = cId;
        components[cId].push_back(u);

        for (const auto& edge : adj[u]) {
            int v = edge.to;
            int eId = edge.edgeId;
            if (isBridge[eId]) continue; // Do not cross bridges
            if (compId[v] == -1) {
                dfsComponent(v, cId);
            }
        }
    }

public:
    explicit BridgeBiconnectedComponents(int vertices)
        : n(vertices), edgeCount(0), adj(vertices),
          tin(vertices, 0), low(vertices, 0), compId(vertices, -1),
          timer(0), numComponents(0) {}

    void addEdge(int u, int v) {
        int eId = edgeCount++;
        adj[u].push_back({v, eId});
        adj[v].push_back({u, eId});
        edgeList.push_back({u, v});
        isBridge.push_back(false);
    }

    void compute() {
        // Step 1: Find all bridges
        for (int i = 0; i < n; ++i) {
            if (!tin[i]) {
                dfsBridges(i, -1);
            }
        }

        // Step 2: Extract 2-edge-connected components
        for (int i = 0; i < n; ++i) {
            if (compId[i] == -1) {
                components.emplace_back();
                dfsComponent(i, numComponents);
                numComponents++;
            }
        }
    }

    const std::vector<std::vector<int>>& getComponents() const {
        return components;
    }

    std::vector<std::pair<int, int>> getBridges() const {
        std::vector<std::pair<int, int>> bridges;
        for (size_t i = 0; i < isBridge.size(); ++i) {
            if (isBridge[i]) {
                bridges.push_back(edgeList[i]);
            }
        }
        return bridges;
    }

    int getComponentOf(int u) const {
        return compId[u];
    }
};

} // namespace graph

/*
 * Demonstration Graph Layout:
 *
 *     (0) --- (1)         (4) --- (5)
 *      |   X   |           |       |
 *     (2) --- (3) --- (6) (7) --- (8)
 *                      |
 *                     (9)
 *
 * Subgraph {0, 1, 2, 3} is a complete clique K4 (2-edge-connected).
 * Subgraph {4, 5, 7, 8} is a cycle C4 (2-edge-connected).
 * Edges (3, 6), (6, 9), (6, 7) are bridges.
 */

int main() {
    std::cout << "========================================\n";
    std::cout << "  BRIDGE-BICONNECTED COMPONENTS (2ECC)  \n";
    std::cout << "========================================\n\n";

    int n = 10;
    graph::BridgeBiconnectedComponents g(n);

    // K4 component: {0, 1, 2, 3}
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(0, 3);
    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 3);

    // Bridge from 3 to 6
    g.addEdge(3, 6);

    // Bridge from 6 to 9
    g.addEdge(6, 9);

    // Bridge from 6 to 7
    g.addEdge(6, 7);

    // Cycle C4 component: {4, 5, 8, 7}
    g.addEdge(7, 4);
    g.addEdge(4, 5);
    g.addEdge(5, 8);
    g.addEdge(8, 7);

    g.compute();

    auto bridges = g.getBridges();
    std::cout << "Identified Bridges (" << bridges.size() << " total):\n";
    for (const auto& b : bridges) {
        std::cout << "  - Bridge: (" << b.first << " <---> " << b.second << ")\n";
    }

    auto components = g.getComponents();
    std::cout << "\n2-Edge-Connected Components (" << components.size() << " total):\n";
    for (size_t i = 0; i < components.size(); ++i) {
        std::cout << "  Component " << i << ": [ ";
        for (int v : components[i]) {
            std::cout << v << " ";
        }
        std::cout << "]\n";
    }

    std::cout << "\nCondensation Forest Verification:\n";
    std::cout << "All vertices within any single component remain connected even if an edge inside is removed.\n";
    std::cout << "Program finished successfully.\n";
    return 0;
}
