/**
 * @file welsh_powell_graph_coloring.cpp
 * @brief Welsh-Powell greedy vertex coloring algorithm in C++17.
 *
 * Overview (Welsh & Powell, 1967):
 *   A classical greedy heuristic algorithm that colors vertices of an undirected graph
 *   such that no two adjacent vertices share the same color, minimizing the total
 *   number of colors used.
 *
 * Algorithm Steps:
 *   1. Calculate the degree of every vertex in G.
 *   2. Sort the vertices in descending order of their degrees:
 *        deg(v_1) >= deg(v_2) >= ... >= deg(v_n).
 *   3. For color c = 0, 1, 2, ...:
 *        Iterate through the sorted vertices. Assign color c to the first uncolored
 *        vertex. Then assign color c to every other uncolored vertex that has NO edge
 *        to any vertex already colored with c.
 *   4. Repeat until all vertices are colored.
 *
 * Theoretical Upper Bound:
 *   chi(G) <= 1 + max_i ( min(deg(v_i), i - 1) ).
 *
 * Complexity:
 *   - Degree calculation & sorting: O(V log V + E).
 *   - Greedy coloring passes: O(V^2 + E).
 *   - Overall time: O(V^2), Space: O(V + E).
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>

namespace graph {

class WelshPowellColoring {
private:
    int n;
    std::vector<std::vector<int>> adj;

public:
    explicit WelshPowellColoring(int vertices) : n(vertices), adj(vertices) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    struct Result {
        std::vector<int> colors; // colors[u] is the color assigned to vertex u
        int numColorsUsed;
        bool isValidColoring;
    };

    Result solve() {
        // 1. Sort vertices by degree descending
        std::vector<std::pair<int, int>> degVertex(n);
        for (int i = 0; i < n; ++i) {
            degVertex[i] = {static_cast<int>(adj[i].size()), i};
        }
        std::sort(degVertex.rbegin(), degVertex.rend());

        std::vector<int> colors(n, -1);
        int currentColor = 0;
        int coloredCount = 0;

        // 2. Greedy assignment passes
        while (coloredCount < n) {
            std::vector<int> currentBatch;

            for (const auto& pair : degVertex) {
                int u = pair.second;
                if (colors[u] != -1) continue;

                // Check if u is adjacent to any vertex in currentBatch
                bool adjacentToBatch = false;
                for (int coloredNode : currentBatch) {
                    if (std::find(adj[u].begin(), adj[u].end(), coloredNode) != adj[u].end()) {
                        adjacentToBatch = true;
                        break;
                    }
                }

                if (!adjacentToBatch) {
                    colors[u] = currentColor;
                    currentBatch.push_back(u);
                    coloredCount++;
                }
            }
            currentColor++;
        }

        // 3. Verify validity: no two adjacent vertices have identical colors
        bool valid = true;
        for (int u = 0; u < n; ++u) {
            for (int v : adj[u]) {
                if (colors[u] == colors[v]) {
                    valid = false;
                    break;
                }
            }
        }

        return {colors, currentColor, valid};
    }
};

} // namespace graph

/*
 * Demonstration Graph (Wheel Graph W_6 with center hub 0 and outer 5-cycle 1-2-3-4-5):
 * Hub 0 is connected to 1, 2, 3, 4, 5 (deg = 5).
 * Outer cycle: 1-2, 2-3, 3-4, 4-5, 5-1 (deg = 3 each).
 * Chromatic number of W_6: Hub needs 1 color, odd 5-cycle needs 3 colors -> chi = 4.
 */

int main() {
    std::cout << "========================================\n";
    std::cout << "      WELSH-POWELL GRAPH COLORING       \n";
    std::cout << "========================================\n\n";

    int n = 6;
    graph::WelshPowellColoring g(n);

    // Hub connections
    for (int i = 1; i <= 5; ++i) {
        g.addEdge(0, i);
    }
    // Outer 5-cycle
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(4, 5);
    g.addEdge(5, 1);

    auto res = g.solve();

    std::cout << "Target Graph: Wheel Graph W_6 (Hub + C_5 odd cycle)\n";
    std::cout << "Total Colors Used: " << res.numColorsUsed << " (Theoretical chi = 4)\n";
    std::cout << "Is Proper Coloring? " << (res.isValidColoring ? "YES (Zero conflicts)" : "NO") << "\n\n";

    std::cout << std::left << std::setw(10) << "Vertex"
              << std::setw(12) << "Assigned Color\n";
    std::cout << std::string(24, '-') << "\n";

    for (int i = 0; i < n; ++i) {
        std::cout << std::left << "Vertex " << std::setw(4) << i
                  << "Color " << res.colors[i] << "\n";
    }

    std::cout << "\nProgram finished successfully.\n";
    return 0;
}
