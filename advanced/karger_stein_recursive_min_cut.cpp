/**
 * @file karger_stein_recursive_min_cut.cpp
 * @brief Karger-Stein Recursive Fast Randomized Minimum Cut Algorithm in C++17.
 *
 * The Karger-Stein algorithm (1996) is a randomized algorithm for computing
 * the Global Minimum Cut of an undirected connected multigraph.
 *
 * Improvement over Basic Karger:
 * - Karger's basic contraction algorithm has success probability ~ 2 / n^2,
 *   requiring O(n^2 log n) trials -> O(n^4) overall time.
 * - Karger-Stein observes that early contractions are much less likely to destroy
 *   the minimum cut than late contractions.
 * - By contracting the graph down to ceil(1 + n / sqrt(2)) vertices and then
 *   branching twice recursively, the recurrence is:
 *     T(n) = 2 * T(ceil(1 + n / sqrt(2))) + O(n^2)
 *   yielding T(n) = O(n^2 log n).
 * - A single run succeeds with probability Omega(1 / log n).
 * - Repeating O(log^2 n) times achieves high probability (1 - 1/n) in O(n^2 log^3 n).
 *
 * Complexity:
 *   - Single recursive trial: O(V^2 log V)
 *   - Space: O(V + E)
 */

#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <iomanip>

namespace graph {

struct Edge {
    int u;
    int v;
};

class DisjointSetUnion {
private:
    std::vector<int> parent;
    std::vector<int> rank;
    int components;

public:
    explicit DisjointSetUnion(int n) : parent(n), rank(n, 0), components(n) {
        for (int i = 0; i < n; ++i) parent[i] = i;
    }

    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]);
    }

    bool unite(int i, int j) {
        int rootI = find(i);
        int rootJ = find(j);
        if (rootI != rootJ) {
            if (rank[rootI] < rank[rootJ]) {
                parent[rootI] = rootJ;
            } else if (rank[rootI] > rank[rootJ]) {
                parent[rootJ] = rootI;
            } else {
                parent[rootJ] = rootI;
                rank[rootI]++;
            }
            components--;
            return true;
        }
        return false;
    }

    int getNumComponents() const {
        return components;
    }
};

struct MultiGraph {
    int numVertices;
    std::vector<Edge> edges;
};

class KargerStein {
private:
    std::mt19937 rng;

    /**
     * @brief Contracts G randomly until targetVertices remain.
     */
    MultiGraph contract(const MultiGraph& g, int targetVertices) {
        DisjointSetUnion dsu(g.numVertices);
        std::vector<Edge> remainingEdges = g.edges;

        while (dsu.getNumComponents() > targetVertices && !remainingEdges.empty()) {
            std::uniform_int_distribution<size_t> dist(0, remainingEdges.size() - 1);
            size_t idx = dist(rng);
            Edge chosen = remainingEdges[idx];
            // Swap with back for O(1) removal
            remainingEdges[idx] = remainingEdges.back();
            remainingEdges.pop_back();

            dsu.unite(chosen.u, chosen.v);
        }

        // Relabel vertices and filter out self-loops
        std::vector<int> compMap(g.numVertices, -1);
        int newVertexCount = 0;
        for (int i = 0; i < g.numVertices; ++i) {
            int root = dsu.find(i);
            if (compMap[root] == -1) {
                compMap[root] = newVertexCount++;
            }
        }

        MultiGraph contracted;
        contracted.numVertices = newVertexCount;
        for (const auto& e : g.edges) {
            int rootU = compMap[dsu.find(e.u)];
            int rootV = compMap[dsu.find(e.v)];
            if (rootU != rootV) {
                contracted.edges.push_back({rootU, rootV});
            }
        }
        return contracted;
    }

    /**
     * @brief Recursive Karger-Stein fast minimum cut.
     */
    int recursiveMinCut(const MultiGraph& g) {
        if (g.numVertices <= 6) {
            // Base case: contract down to 2 vertices directly
            auto baseGraph = contract(g, 2);
            return static_cast<int>(baseGraph.edges.size());
        }

        int target = static_cast<int>(std::ceil(1.0 + static_cast<double>(g.numVertices) / std::sqrt(2.0)));

        MultiGraph g1 = contract(g, target);
        int cut1 = recursiveMinCut(g1);

        MultiGraph g2 = contract(g, target);
        int cut2 = recursiveMinCut(g2);

        return std::min(cut1, cut2);
    }

public:
    explicit KargerStein(uint32_t seed = 42) : rng(seed) {}

    /**
     * @brief Computes global minimum cut by repeating recursive trials.
     * @param g Input multigraph.
     * @param trials Number of independent repetitions.
     * @return Global minimum cut size.
     */
    int findMinCut(const MultiGraph& g, int trials = 20) {
        int bestCut = static_cast<int>(g.edges.size());
        for (int i = 0; i < trials; ++i) {
            int cut = recursiveMinCut(g);
            bestCut = std::min(bestCut, cut);
        }
        return bestCut;
    }
};

} // namespace graph

/*
 * Demonstration Graph: Two K4 cliques joined by 2 bottleneck bridge edges.
 * Clique A: {0, 1, 2, 3} (6 edges)
 * Clique B: {4, 5, 6, 7} (6 edges)
 * Cut edges: (3, 4) and (2, 5) -> Min cut capacity is 2.
 */

int main() {
    std::cout << "========================================\n";
    std::cout << "   KARGER-STEIN RECURSIVE MIN-CUT       \n";
    std::cout << "========================================\n\n";

    graph::MultiGraph g;
    g.numVertices = 8;

    auto addClique = [&](int start) {
        for (int i = start; i < start + 4; ++i) {
            for (int j = i + 1; j < start + 4; ++j) {
                g.edges.push_back({i, j});
            }
        }
    };

    // Add Clique A {0..3} and Clique B {4..7}
    addClique(0);
    addClique(4);

    // Bottleneck bridge edges between cliques
    g.edges.push_back({3, 4});
    g.edges.push_back({2, 5});

    std::cout << "Input Graph Architecture:\n";
    std::cout << "  - Total vertices: " << g.numVertices << "\n";
    std::cout << "  - Total edges   : " << g.edges.size() << " (Two K_4 cliques + 2 bottleneck bridge edges)\n";
    std::cout << "  - Theoretical Minimum Cut: 2\n\n";

    graph::KargerStein solver(2026);
    int computedMinCut = solver.findMinCut(g, 15);

    std::cout << "Karger-Stein Recursive Contraction Result:\n";
    std::cout << "  - Estimated Global Min Cut: " << computedMinCut << "\n";
    std::cout << "  - Status: " << (computedMinCut == 2 ? "OPTIMAL MATCH" : "SUBOPTIMAL") << "\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
