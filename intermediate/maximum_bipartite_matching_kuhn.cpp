/**
 * @file maximum_bipartite_matching_kuhn.cpp
 * @brief Computes Maximum Cardinality Bipartite Matching and Minimum Vertex Cover via Kuhn's Algorithm.
 *
 * Theory:
 * 1. Bipartite Matching (Kuhn's Algorithm, 1955):
 *    - Left partition U (size n), Right partition V (size m).
 *    - Finds augmenting paths using Depth-First Search.
 *    - Reverses matching edges along alternating paths.
 *    - Guaranteed maximum cardinality matching by Berge's Lemma.
 *
 * 2. Konig's Theorem:
 *    - In any bipartite graph: |Maximum Matching| == |Minimum Vertex Cover|.
 *    - Minimum vertex cover can be extracted by finding all vertices reachable via
 *      alternating paths from unmatched vertices in U:
 *        Cover = (U \ Reachable_U) union (V intersect Reachable_V).
 *
 * Time Complexity: O(V * E)
 * Space Complexity: O(V + E)
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>

struct BipartiteMatchingResult {
    int maxMatchingSize;
    std::vector<std::pair<int, int>> matchedEdges; // (u, v)
    std::vector<int> minVertexCoverU;
    std::vector<int> minVertexCoverV;
};

class KuhnBipartiteMatcher {
private:
    int n, m; // |U| and |V|
    std::vector<std::vector<int>> adj; // adj[u] contains vertices in V
    std::vector<int> matchToU;        // matchToU[v] = u (where v in V, u in U)
    std::vector<int> matchToV;        // matchToV[u] = v
    std::vector<bool> visited;

    bool dfs(int u) {
        for (int v : adj[u]) {
            if (visited[v]) continue;
            visited[v] = true;

            if (matchToU[v] == -1 || dfs(matchToU[v])) {
                matchToU[v] = u;
                matchToV[u] = v;
                return true;
            }
        }
        return false;
    }

    void dfsAlternating(int u, std::vector<bool>& visU, std::vector<bool>& visV) {
        visU[u] = true;
        for (int v : adj[u]) {
            if (!visV[v] && matchToU[v] != -1 && matchToU[v] != u) {
                visV[v] = true;
                dfsAlternating(matchToU[v], visU, visV);
            }
        }
    }

public:
    KuhnBipartiteMatcher(int sizeU, int sizeV)
        : n(sizeU), m(sizeV), adj(sizeU), matchToU(sizeV, -1), matchToV(sizeU, -1), visited(sizeV, false) {}

    void addEdge(int u, int v) {
        if (u >= 0 && u < n && v >= 0 && v < m) {
            adj[u].push_back(v);
        }
    }

    BipartiteMatchingResult solve() {
        int matchingSize = 0;
        for (int u = 0; u < n; ++u) {
            std::fill(visited.begin(), visited.end(), false);
            if (dfs(u)) {
                matchingSize++;
            }
        }

        std::vector<std::pair<int, int>> matches;
        for (int u = 0; u < n; ++u) {
            if (matchToV[u] != -1) {
                matches.push_back({u, matchToV[u]});
            }
        }

        // Konig's Theorem: Minimum Vertex Cover
        std::vector<bool> reachableU(n, false);
        std::vector<bool> reachableV(m, false);

        for (int u = 0; u < n; ++u) {
            if (matchToV[u] == -1) {
                dfsAlternating(u, reachableU, reachableV);
            }
        }

        std::vector<int> coverU;
        for (int u = 0; u < n; ++u) {
            if (!reachableU[u]) coverU.push_back(u);
        }

        std::vector<int> coverV;
        for (int v = 0; v < m; ++v) {
            if (reachableV[v]) coverV.push_back(v);
        }

        return {matchingSize, matches, coverU, coverV};
    }
};

int main() {
    std::cout << "=== Kuhn's Algorithm (Maximum Bipartite Matching & Konig's Cover) ===\n\n";

    // Bipartite Graph: Left U = {0, 1, 2, 3}, Right V = {0, 1, 2, 3}
    // Job Applicant Matching:
    // Applicant 0 can do Job 0, Job 1
    // Applicant 1 can do Job 1, Job 2
    // Applicant 2 can do Job 0, Job 2
    // Applicant 3 can do Job 2, Job 3
    int n = 4, m = 4;
    KuhnBipartiteMatcher matcher(n, m);
    matcher.addEdge(0, 0);
    matcher.addEdge(0, 1);
    matcher.addEdge(1, 1);
    matcher.addEdge(1, 2);
    matcher.addEdge(2, 0);
    matcher.addEdge(2, 2);
    matcher.addEdge(3, 2);
    matcher.addEdge(3, 3);

    auto res = matcher.solve();

    std::cout << "Maximum Matching Size: " << res.maxMatchingSize << '\n';
    std::cout << "Matched Pairs (U -> V):\n";
    for (const auto& [u, v] : res.matchedEdges) {
        std::cout << "  Applicant " << u << " -> Job " << v << '\n';
    }

    std::cout << "\nKonig's Minimum Vertex Cover (Size = "
              << (res.minVertexCoverU.size() + res.minVertexCoverV.size()) << "):\n";
    std::cout << "  Left Vertices (U):  { ";
    for (int u : res.minVertexCoverU) std::cout << u << " ";
    std::cout << "}\n";
    std::cout << "  Right Vertices (V): { ";
    for (int v : res.minVertexCoverV) std::cout << v << " ";
    std::cout << "}\n";

    return 0;
}

