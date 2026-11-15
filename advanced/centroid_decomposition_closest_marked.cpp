/*
 * ============================================================================
 *  Centroid Decomposition — Point Update & Path Distance Queries
 * ============================================================================
 *
 *  Centroid decomposition recursively partitions a tree by repeatedly finding
 *  and removing centroids. The resulting "centroid tree" has O(log N) depth,
 *  enabling efficient answering of queries involving all paths through any node.
 *
 *  Algorithm:
 *  1) Find the centroid c of the current tree (node whose removal leaves no
 *     subtree larger than N/2).
 *  2) Mark c as the root of the current centroid subtree.
 *  3) Remove c and recurse on each remaining connected component.
 *  4) The centroid tree has depth O(log N).
 *
 *  Application Demonstrated:
 *  "Given a weighted tree, answer queries: find the closest marked node to a
 *  query node." We support:
 *    - mark(v):   Mark node v.
 *    - closest(v): Find the minimum distance from v to any marked node.
 *
 *  Using centroid decomposition, both operations run in O(log^2 N) time
 *  (O(log N) ancestors in the centroid tree, O(log N) for distance via LCA).
 *
 *  For simplicity, we precompute distances to centroid ancestors during
 *  decomposition and use arrays instead of LCA.
 *
 *  Time Complexity:
 *  - Decomposition: O(N log N)
 *  - mark / closest: O(log N) per operation
 *  Space Complexity: O(N log N)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <queue>
#include <cstdint>
#include <algorithm>
#include <limits>
#include <iomanip>

namespace centroid_decomp {

constexpr int64_t INF = std::numeric_limits<int64_t>::max();

class CentroidDecomposition {
public:
    CentroidDecomposition(int n)
        : numNodes(n), adj(n), removed(n, false), subtreeSize(n, 0),
          centroidParent(n, -1), bestDist(n, INF) {
        ancestors.resize(n);
    }

    void addEdge(int u, int v, int64_t w) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    void build() {
        computeSubtreeSize(0, -1);
        decompose(0, -1);
    }

    /* Mark node v and update closest-marked distances for all centroid ancestors */
    void mark(int v) {
        /* Update v's own record */
        bestDist[v] = 0;

        /* Walk up centroid tree, updating each ancestor */
        for (auto& [ancestor, dist] : ancestors[v]) {
            bestDist[ancestor] = std::min(bestDist[ancestor], dist);
        }
    }

    /* Query: minimum distance from v to any marked node */
    int64_t closest(int v) const {
        int64_t result = bestDist[v]; /* If v itself is marked, distance = 0 */

        for (const auto& [ancestor, dist] : ancestors[v]) {
            if (bestDist[ancestor] < INF) {
                result = std::min(result, dist + bestDist[ancestor]);
            }
        }

        return result;
    }

private:
    struct Edge {
        int to;
        int64_t weight;
    };

    int numNodes;
    std::vector<std::vector<Edge>> adj;
    std::vector<bool> removed;
    std::vector<int> subtreeSize;
    std::vector<int> centroidParent;
    std::vector<int64_t> bestDist;

    /* For each node, list of (centroid_ancestor, distance_to_ancestor) */
    std::vector<std::vector<std::pair<int, int64_t>>> ancestors;

    void computeSubtreeSize(int v, int parent) {
        subtreeSize[v] = 1;
        for (auto& e : adj[v]) {
            if (e.to != parent && !removed[e.to]) {
                computeSubtreeSize(e.to, v);
                subtreeSize[v] += subtreeSize[e.to];
            }
        }
    }

    int findCentroid(int v, int parent, int treeSize) {
        for (auto& e : adj[v]) {
            if (e.to != parent && !removed[e.to] && subtreeSize[e.to] > treeSize / 2) {
                return findCentroid(e.to, v, treeSize);
            }
        }
        return v;
    }

    /* BFS to compute distances from centroid c to all nodes in its component */
    void computeDistances(int centroid) {
        std::queue<std::pair<int, int>> bfsQueue;
        std::vector<int64_t> dist(numNodes, -1);
        dist[centroid] = 0;
        bfsQueue.push({centroid, -1});

        while (!bfsQueue.empty()) {
            auto [v, par] = bfsQueue.front();
            bfsQueue.pop();

            if (v != centroid) {
                ancestors[v].push_back({centroid, dist[v]});
            }

            for (auto& e : adj[v]) {
                if (e.to != par && !removed[e.to] && dist[e.to] == -1) {
                    dist[e.to] = dist[v] + e.weight;
                    bfsQueue.push({e.to, v});
                }
            }
        }
    }

    void decompose(int v, int par) {
        computeSubtreeSize(v, -1);
        int c = findCentroid(v, -1, subtreeSize[v]);
        centroidParent[c] = par;
        removed[c] = true;

        computeDistances(c);

        for (auto& e : adj[c]) {
            if (!removed[e.to]) {
                decompose(e.to, c);
            }
        }
    }
};

} // namespace centroid_decomp

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Centroid Decomposition — Closest Marked Node Queries\n";
    std::cout << "========================================================\n\n";

    /*
     * Tree (edge weights in parentheses):
     *
     *       0 --(3)-- 1 --(2)-- 2
     *       |                    |
     *      (5)                  (1)
     *       |                    |
     *       3 --(4)-- 4         5
     *       |
     *      (6)
     *       |
     *       6 --(7)-- 7
     */

    int n = 8;
    centroid_decomp::CentroidDecomposition cd(n);

    cd.addEdge(0, 1, 3);
    cd.addEdge(1, 2, 2);
    cd.addEdge(0, 3, 5);
    cd.addEdge(3, 4, 4);
    cd.addEdge(2, 5, 1);
    cd.addEdge(3, 6, 6);
    cd.addEdge(6, 7, 7);

    cd.build();

    std::cout << "Tree built with " << n << " nodes.\n\n";

    /* Mark node 5 and query */
    std::cout << "Operation: mark(5)\n";
    cd.mark(5);

    std::cout << "Closest marked node distances:\n";
    for (int v = 0; v < n; ++v) {
        int64_t d = cd.closest(v);
        std::cout << "  closest(" << v << ") = "
                  << (d >= centroid_decomp::INF ? -1 : d) << "\n";
    }

    /* Mark node 7 */
    std::cout << "\nOperation: mark(7)\n";
    cd.mark(7);

    std::cout << "Closest marked node distances:\n";
    for (int v = 0; v < n; ++v) {
        int64_t d = cd.closest(v);
        std::cout << "  closest(" << v << ") = "
                  << (d >= centroid_decomp::INF ? -1 : d) << "\n";
    }

    /* Mark node 0 */
    std::cout << "\nOperation: mark(0)\n";
    cd.mark(0);

    std::cout << "Closest marked node distances:\n";
    for (int v = 0; v < n; ++v) {
        int64_t d = cd.closest(v);
        std::cout << "  closest(" << v << ") = "
                  << (d >= centroid_decomp::INF ? -1 : d) << "\n";
    }

    /* Verify node 0 distance to itself */
    std::cout << "\nSelf-distance check: closest(0) = " << cd.closest(0)
              << " (expected 0)\n";
    std::cout << "Verification: " << (cd.closest(0) == 0 ? "PASSED" : "FAILED") << "\n";

    return 0;
}
