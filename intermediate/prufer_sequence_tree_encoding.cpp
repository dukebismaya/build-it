/**
 * @file prufer_sequence_tree_encoding.cpp
 * @brief Encodes a labeled tree into a Prufer sequence and reconstructs the tree from the sequence.
 *
 * Theory:
 * Invented by Heinz Prufer in 1918 to provide a bijective proof of Cayley's Formula,
 * which states that there are N^(N-2) distinct labeled trees on N vertices.
 *
 * Properties:
 * 1. For any tree with N vertices (labeled 0 to N-1), its Prufer sequence has length N - 2.
 * 2. Every sequence of N - 2 integers from {0, ..., N-1} corresponds to a unique tree.
 * 3. A vertex of degree d appears exactly d - 1 times in the sequence. Leaves (degree 1) do not appear initially.
 *
 * Algorithms:
 * - Encoding:
 *   Repeatedly remove the leaf with the smallest label and record its neighbor, until 2 vertices remain.
 * - Decoding:
 *   Compute vertex degrees (deg[v] = 1 + occurrences in sequence).
 *   Iterate through the sequence, connecting each element to the lowest-labeled vertex with degree 1.
 *   Finally, connect the last two remaining vertices with degree 1.
 *
 * Time Complexity:
 * - Encoding: O(N log N) using a min-priority queue
 * - Decoding: O(N log N) using a min-priority queue
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <iomanip>

class PruferCodec {
public:
    /**
     * @brief Encodes a labeled tree with n vertices (0 to n-1) into a Prufer sequence.
     * @param n Number of vertices.
     * @param edges Vector of undirected edges (u, v).
     * @return Prufer sequence of length n - 2.
     */
    static std::vector<int> encode(int n, const std::vector<std::pair<int, int>>& edges) {
        if (n <= 2) return {};

        std::vector<std::set<int>> adj(n);
        std::vector<int> degree(n, 0);

        for (const auto& [u, v] : edges) {
            adj[u].insert(v);
            adj[v].insert(u);
            degree[u]++;
            degree[v]++;
        }

        std::priority_queue<int, std::vector<int>, std::greater<int>> leaves;
        for (int i = 0; i < n; ++i) {
            if (degree[i] == 1) {
                leaves.push(i);
            }
        }

        std::vector<int> prufer;
        prufer.reserve(n - 2);

        for (int iter = 0; iter < n - 2; ++iter) {
            int leaf = leaves.top();
            leaves.pop();

            int neighbor = *adj[leaf].begin();
            prufer.push_back(neighbor);

            adj[leaf].erase(neighbor);
            adj[neighbor].erase(leaf);

            degree[leaf]--;
            degree[neighbor]--;

            if (degree[neighbor] == 1) {
                leaves.push(neighbor);
            }
        }

        return prufer;
    }

    /**
     * @brief Decodes a Prufer sequence of length n - 2 into the original tree edges.
     * @param n Number of vertices.
     * @param prufer Sequence of length n - 2.
     * @return Vector of reconstructed tree edges.
     */
    static std::vector<std::pair<int, int>> decode(int n, const std::vector<int>& prufer) {
        if (n <= 1) return {};
        if (n == 2) return {{0, 1}};

        std::vector<int> degree(n, 1);
        for (int v : prufer) {
            degree[v]++;
        }

        std::priority_queue<int, std::vector<int>, std::greater<int>> leaves;
        for (int i = 0; i < n; ++i) {
            if (degree[i] == 1) {
                leaves.push(i);
            }
        }

        std::vector<std::pair<int, int>> edges;
        edges.reserve(n - 1);

        for (int v : prufer) {
            int leaf = leaves.top();
            leaves.pop();

            edges.push_back({std::min(leaf, v), std::max(leaf, v)});

            degree[leaf]--;
            degree[v]--;

            if (degree[v] == 1) {
                leaves.push(v);
            }
        }

        // Connect the last two remaining vertices with degree 1
        int u = leaves.top();
        leaves.pop();
        int v = leaves.top();
        leaves.pop();
        edges.push_back({std::min(u, v), std::max(u, v)});

        std::sort(edges.begin(), edges.end());
        return edges;
    }
};

void printEdges(const std::string& label, const std::vector<std::pair<int, int>>& edges) {
    std::cout << label << ": ";
    for (size_t i = 0; i < edges.size(); ++i) {
        std::cout << "(" << edges[i].first << "-" << edges[i].second << ")";
        if (i + 1 < edges.size()) std::cout << ", ";
    }
    std::cout << '\n';
}

int main() {
    std::cout << "=== Prufer Sequence Tree Encoding & Decoding ===\n\n";

    // Tree with 6 vertices (0 to 5):
    // Edges: (0, 3), (1, 3), (2, 3), (3, 4), (4, 5)
    // Leaf removal trace:
    // 1. Min leaf is 0 -> neighbor 3 (Prufer: [3])
    // 2. Min leaf is 1 -> neighbor 3 (Prufer: [3, 3])
    // 3. Min leaf is 2 -> neighbor 3 (Prufer: [3, 3, 3])
    // 4. Min leaf is 3 -> neighbor 4 (Prufer: [3, 3, 3, 4])
    // Remaining vertices: 4, 5
    int n = 6;
    std::vector<std::pair<int, int>> originalEdges = {
        {0, 3}, {1, 3}, {2, 3}, {3, 4}, {4, 5}
    };
    std::sort(originalEdges.begin(), originalEdges.end());

    std::cout << "1. Original Tree (N = " << n << "):\n";
    printEdges("  Edges", originalEdges);

    auto pruferSeq = PruferCodec::encode(n, originalEdges);
    std::cout << "\n2. Encoded Prufer Sequence (Length " << pruferSeq.size() << "):\n  [ ";
    for (int v : pruferSeq) std::cout << v << " ";
    std::cout << "]\n\n";

    auto decodedEdges = PruferCodec::decode(n, pruferSeq);
    std::cout << "3. Decoded Reconstructed Tree:\n";
    printEdges("  Edges", decodedEdges);

    bool match = (originalEdges == decodedEdges);
    std::cout << "\nRound-trip Encoding & Decoding Match? " << (match ? "PASS (Exact Bijection)" : "FAIL") << '\n';

    return 0;
}

