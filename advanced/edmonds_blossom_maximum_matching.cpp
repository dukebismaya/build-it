/**
 * @file edmonds_blossom_maximum_matching.cpp
 * @brief Computes Maximum Cardinality Matching in general (non-bipartite) graphs using Edmonds' Blossom Algorithm.
 *
 * Concepts:
 * - A matching M in graph G is a set of edges without common vertices.
 * - In bipartite graphs, augmenting paths can be found via standard BFS (e.g., Hopcroft-Karp).
 * - In general graphs, odd-length cycles can cause standard alternating path searches to fail.
 * - Jack Edmonds (1965) proved that any odd-length cycle (called a "blossom") can be shrunk into
 *   a single pseudo-vertex without altering the existence of an augmenting path.
 * - Finding Lowest Common Ancestor (LCA) in the alternating BFS forest identifies the blossom base.
 * - Shrinking blossoms via Disjoint Set Union (DSU) allows BFS to cross odd cycles and uncover augmenting paths.
 *
 * Time Complexity: O(V^3)
 * Space Complexity: O(V + E)
 */

#include <iostream>
#include <vector>
#include <queue>
#include <iomanip>
#include <algorithm>

class BlossomMaximumMatching {
private:
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<int> match;   // match[u] = partner of u, or -1 if unmatched
    std::vector<int> parent;  // parent in BFS tree
    std::vector<int> base;    // base of the blossom containing vertex u
    std::vector<int> dsu;     // DSU parent for blossom shrinking
    std::vector<bool> inQueue;
    std::vector<bool> inBlossom;

    int find(int u) {
        if (dsu[u] == u) return u;
        return dsu[u] = find(dsu[u]);
    }

    int lca(int root, int u, int v) {
        std::vector<bool> visited(n, false);
        while (true) {
            u = base[u];
            visited[u] = true;
            if (u == root) break;
            u = parent[match[u]];
        }
        while (true) {
            v = base[v];
            if (visited[v]) return v;
            v = parent[match[v]];
        }
    }

    void markBlossom(int lcaNode, int u) {
        while (base[u] != lcaNode) {
            int v = match[u];
            inBlossom[base[u]] = true;
            inBlossom[base[v]] = true;
            u = parent[v];
            if (base[u] != lcaNode) {
                parent[u] = v;
            }
        }
    }

    void blossomShrink(int lcaNode, int u, int v, std::queue<int>& q) {
        std::fill(inBlossom.begin(), inBlossom.end(), false);
        markBlossom(lcaNode, u);
        markBlossom(lcaNode, v);

        if (base[u] != lcaNode) parent[u] = v;
        if (base[v] != lcaNode) parent[v] = u;

        for (int i = 0; i < n; ++i) {
            if (inBlossom[base[i]]) {
                base[i] = lcaNode;
                dsu[i] = lcaNode;
                if (!inQueue[i]) {
                    q.push(i);
                    inQueue[i] = true;
                }
            }
        }
    }

    bool bfsAugment(int root) {
        std::fill(parent.begin(), parent.end(), -1);
        std::fill(inQueue.begin(), inQueue.end(), false);
        for (int i = 0; i < n; ++i) {
            dsu[i] = i;
            base[i] = i;
        }

        std::queue<int> q;
        q.push(root);
        inQueue[root] = true;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (int v : adj[u]) {
                if (base[u] == base[v] || match[u] == v) continue;

                if (v == root || (match[v] != -1 && parent[match[v]] != -1)) {
                    // Blossom detected (odd-length cycle)
                    int curLca = lca(root, u, v);
                    blossomShrink(curLca, u, v, q);
                } else if (parent[v] == -1) {
                    parent[v] = u;
                    if (match[v] == -1) {
                        // Augmenting path found! Unroll and flip matching edges
                        int curr = v;
                        while (curr != -1) {
                            int prev = parent[curr];
                            int nextMatch = (prev != -1) ? match[prev] : -1;
                            match[curr] = prev;
                            match[prev] = curr;
                            curr = nextMatch;
                        }
                        return true;
                    }

                    int w = match[v];
                    parent[w] = v;
                    if (!inQueue[w]) {
                        q.push(w);
                        inQueue[w] = true;
                    }
                }
            }
        }

        return false;
    }

public:
    explicit BlossomMaximumMatching(int vertices)
        : n(vertices),
          adj(vertices),
          match(vertices, -1),
          parent(vertices, -1),
          base(vertices),
          dsu(vertices),
          inQueue(vertices, false),
          inBlossom(vertices, false) {}

    void addEdge(int u, int v) {
        if (u >= 0 && u < n && v >= 0 && v < n && u != v) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }

    int computeMaximumMatching() {
        int matchingSize = 0;
        for (int i = 0; i < n; ++i) {
            if (match[i] == -1) {
                if (bfsAugment(i)) {
                    matchingSize++;
                }
            }
        }
        return matchingSize;
    }

    const std::vector<int>& getMatching() const {
        return match;
    }
};

int main() {
    std::cout << "=== Edmonds' Blossom Algorithm (General Graph Maximum Matching) ===\n\n";

    // Example 1: Triangle C3 with an attached whisker (Odd cycle graph)
    // Vertices: 0, 1, 2 form a triangle. 2 connects to 3.
    // 0-1, 1-2, 2-0, 2-3
    // Max matching should match (0, 1) and (2, 3) -> size 2
    {
        std::cout << "Example 1: Triangle with whisker (4 vertices)\n";
        BlossomMaximumMatching b1(4);
        b1.addEdge(0, 1);
        b1.addEdge(1, 2);
        b1.addEdge(2, 0);
        b1.addEdge(2, 3);

        int maxMatch = b1.computeMaximumMatching();
        std::cout << "  Maximum Matching Size: " << maxMatch << '\n';
        std::cout << "  Matched Pairs: ";
        auto m = b1.getMatching();
        for (int i = 0; i < 4; ++i) {
            if (m[i] > i) {
                std::cout << "(" << i << " - " << m[i] << ") ";
            }
        }
        std::cout << "\n\n";
    }

    // Example 2: 5-Cycle C5 (Pentagon - Non-bipartite)
    // 0-1, 1-2, 2-3, 3-4, 4-0
    // Max matching in C5 is 2 edges (e.g., (0,1), (2,3))
    {
        std::cout << "Example 2: 5-Cycle C5 (Pentagon)\n";
        BlossomMaximumMatching b2(5);
        for (int i = 0; i < 5; ++i) {
            b2.addEdge(i, (i + 1) % 5);
        }

        int maxMatch = b2.computeMaximumMatching();
        std::cout << "  Maximum Matching Size: " << maxMatch << '\n';
        std::cout << "  Matched Pairs: ";
        auto m = b2.getMatching();
        for (int i = 0; i < 5; ++i) {
            if (m[i] > i) {
                std::cout << "(" << i << " - " << m[i] << ") ";
            }
        }
        std::cout << "\n\n";
    }

    // Example 3: Petersen Graph snippet (10 vertices, 3-regular non-bipartite)
    // Outer 5-cycle: 0..4, Inner star: 5..9
    // Perfect matching of size 5 exists!
    {
        std::cout << "Example 3: Petersen Graph (10 vertices, 15 edges)\n";
        BlossomMaximumMatching petersen(10);
        // Outer cycle
        for (int i = 0; i < 5; ++i) {
            petersen.addEdge(i, (i + 1) % 5);
        }
        // Inner star
        petersen.addEdge(5, 7);
        petersen.addEdge(7, 9);
        petersen.addEdge(9, 6);
        petersen.addEdge(6, 8);
        petersen.addEdge(8, 5);
        // Cross edges
        for (int i = 0; i < 5; ++i) {
            petersen.addEdge(i, i + 5);
        }

        int maxMatch = petersen.computeMaximumMatching();
        std::cout << "  Maximum Matching Size: " << maxMatch << " (Perfect matching expected: 5)\n";
        std::cout << "  Matched Pairs: ";
        auto m = petersen.getMatching();
        for (int i = 0; i < 10; ++i) {
            if (m[i] > i) {
                std::cout << "(" << i << " - " << m[i] << ") ";
            }
        }
        std::cout << '\n';
    }

    return 0;
}
