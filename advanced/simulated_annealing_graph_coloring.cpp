/**
 * @file simulated_annealing_graph_coloring.cpp
 * @brief Simulated Annealing Metaheuristic for the Graph Vertex Coloring Problem in C++17.
 *
 * Graph Vertex Coloring is an NP-hard problem: assign each vertex v in V a color
 * in {0, 1, ..., K-1} such that no two adjacent vertices share the same color.
 *
 * Simulated Annealing (SA) Formulation:
 * - State Space: A vector color[u] in {0, ..., K-1} for all vertices u.
 * - Energy Function E(S): Total number of edge conflicts (adjacent vertices sharing a color):
 *     E(S) = sum_{(u, v) in E} [ color[u] == color[v] ]
 * - Neighborhood Move: Select a conflicted vertex u and assign it an alternative color c'.
 * - Metropolis Criterion:
 *     If delta_E <= 0: accept move unconditionally.
 *     If delta_E > 0: accept with probability P = exp(-delta_E / T).
 * - Cooling Schedule:
 *     Geometric cooling: T_{k+1} = alpha * T_k, with alpha in [0.95, 0.999].
 *
 * Complexity:
 *   - Move evaluation: O(deg(u)) time.
 *   - Total run: O(iter * max_deg) time, O(V + E) space.
 */

#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <iomanip>
#include <algorithm>

namespace metaheuristic {

struct Graph {
    int numVertices;
    std::vector<std::vector<int>> adj;

    explicit Graph(int n) : numVertices(n), adj(n) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
};

class SimulatedAnnealingColoring {
private:
    const Graph& graph;
    int numColors;
    std::mt19937 rng;

    int computeTotalConflicts(const std::vector<int>& colors) {
        int conflicts = 0;
        for (int u = 0; u < graph.numVertices; ++u) {
            for (int v : graph.adj[u]) {
                if (u < v && colors[u] == colors[v]) {
                    conflicts++;
                }
            }
        }
        return conflicts;
    }

    int countVertexConflicts(int u, int color, const std::vector<int>& colors) {
        int cnt = 0;
        for (int v : graph.adj[u]) {
            if (colors[v] == color) {
                cnt++;
            }
        }
        return cnt;
    }

public:
    SimulatedAnnealingColoring(const Graph& g, int k, uint32_t seed = 42)
        : graph(g), numColors(k), rng(seed) {}

    struct Result {
        bool success;
        int finalConflicts;
        std::vector<int> colors;
        int iterations;
    };

    Result solve(double initialTemp = 10.0, double finalTemp = 0.01, double alpha = 0.995, int maxStepsPerTemp = 200) {
        std::uniform_int_distribution<int> colorDist(0, numColors - 1);
        std::uniform_int_distribution<int> vertexDist(0, graph.numVertices - 1);
        std::uniform_real_distribution<double> probDist(0.0, 1.0);

        // Initial random coloring
        std::vector<int> currentColors(graph.numVertices);
        for (int i = 0; i < graph.numVertices; ++i) {
            currentColors[i] = colorDist(rng);
        }

        int currentConflicts = computeTotalConflicts(currentColors);
        std::vector<int> bestColors = currentColors;
        int bestConflicts = currentConflicts;

        double temp = initialTemp;
        int totalIterations = 0;

        while (temp > finalTemp && bestConflicts > 0) {
            for (int step = 0; step < maxStepsPerTemp; ++step) {
                totalIterations++;

                // Pick a random vertex
                int u = vertexDist(rng);
                int oldColor = currentColors[u];
                int newColor = colorDist(rng);
                if (newColor == oldColor) continue;

                // Evaluate change in conflicts
                int conflictsBefore = countVertexConflicts(u, oldColor, currentColors);
                int conflictsAfter = countVertexConflicts(u, newColor, currentColors);
                int deltaE = conflictsAfter - conflictsBefore;

                if (deltaE <= 0 || probDist(rng) < std::exp(-static_cast<double>(deltaE) / temp)) {
                    currentColors[u] = newColor;
                    currentConflicts += deltaE;

                    if (currentConflicts < bestConflicts) {
                        bestConflicts = currentConflicts;
                        bestColors = currentColors;
                        if (bestConflicts == 0) {
                            return {true, 0, bestColors, totalIterations};
                        }
                    }
                }
            }
            temp *= alpha;
        }

        return {bestConflicts == 0, bestConflicts, bestColors, totalIterations};
    }
};

} // namespace metaheuristic

int main() {
    std::cout << "========================================\n";
    std::cout << " SIMULATED ANNEALING GRAPH COLORING     \n";
    std::cout << "========================================\n\n";

    // Petersen Graph: 10 vertices, 15 edges.
    // Chromatic number of Petersen graph is 3.
    metaheuristic::Graph petersen(10);
    // Outer 5-cycle: 0-1-2-3-4
    petersen.addEdge(0, 1);
    petersen.addEdge(1, 2);
    petersen.addEdge(2, 3);
    petersen.addEdge(3, 4);
    petersen.addEdge(4, 0);

    // Inner 5-star: 5-7, 7-9, 9-6, 6-8, 8-5
    petersen.addEdge(5, 7);
    petersen.addEdge(7, 9);
    petersen.addEdge(9, 6);
    petersen.addEdge(6, 8);
    petersen.addEdge(8, 5);

    // Spokes connecting outer and inner
    petersen.addEdge(0, 5);
    petersen.addEdge(1, 6);
    petersen.addEdge(2, 7);
    petersen.addEdge(3, 8);
    petersen.addEdge(4, 9);

    int kColors = 3;
    std::cout << "Target Graph: Petersen Graph (V = 10, E = 15)\n";
    std::cout << "Searching for valid " << kColors << "-coloring via Simulated Annealing...\n\n";

    metaheuristic::SimulatedAnnealingColoring sa(petersen, kColors, 1337);
    auto res = sa.solve(15.0, 0.001, 0.99, 150);

    std::cout << "Optimization Finished in " << res.iterations << " iterations.\n";
    std::cout << "Valid Coloring Found? " << (res.success ? "YES (Zero conflicts)" : "NO") << "\n";
    std::cout << "Remaining Conflicts: " << res.finalConflicts << "\n\n";

    std::cout << "Vertex Color Assignments:\n";
    for (int i = 0; i < 10; ++i) {
        std::cout << "  Vertex " << i << " -> Color " << res.colors[i] << "\n";
    }

    std::cout << "\nProgram finished successfully.\n";
    return 0;
}
