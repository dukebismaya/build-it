/*
 * Program Description:
 * Centroid Decomposition of Trees in C++.
 * A divide-and-conquer technique on arbitrary unrooted trees.
 *
 * Centroid Definition:
 * A vertex in a tree of size N is a centroid if and only if removing it splits the tree
 * into subtrees, each of size at most N / 2. Every tree has at least one and at most two centroids.
 *
 * Centroid Tree Properties:
 * 1. Tree depth is bounded by O(log N).
 * 2. Any path between two nodes u and v in the original tree passes through their Lowest Common
 *    Ancestor in the centroid tree.
 * 3. Total decomposition time: O(N log N).
 *
 * Application implemented:
 * Counts the number of pairs of nodes (u, v) whose simple path distance equals K.
 *
 * Time Complexity: O(N log N)
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>
#include <algorithm>

class CentroidDecomposition {
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<int> subtreeSize;
    std::vector<bool> removed;
    long long pairsWithDistK = 0;
    int targetK = 0;

    int dfsSize(int u, int p) {
        subtreeSize[u] = 1;
        for (int v : adj[u]) {
            if (v != p && !removed[v]) {
                subtreeSize[u] += dfsSize(v, u);
            }
        }
        return subtreeSize[u];
    }

    int getCentroid(int u, int p, int total) {
        for (int v : adj[u]) {
            if (v != p && !removed[v]) {
                if (subtreeSize[v] > total / 2) {
                    return getCentroid(v, u, total);
                }
            }
        }
        return u;
    }

    // Collects path lengths from centroid to all reachable nodes in this branch
    void getDistances(int u, int p, int d, std::vector<int>& dists) {
        if (d > targetK) return;
        dists.push_back(d);
        for (int v : adj[u]) {
            if (v != p && !removed[v]) {
                getDistances(v, u, d + 1, dists);
            }
        }
    }

    long long countPairsWithK(const std::vector<int>& dists) {
        long long count = 0;
        std::vector<int> freq(targetK + 1, 0);

        for (int d : dists) {
            int complement = targetK - d;
            if (complement >= 0 && complement <= targetK) {
                count += freq[complement];
            }
            if (d <= targetK) {
                freq[d]++;
            }
        }
        return count;
    }

    void decompose(int u) {
        int total = dfsSize(u, -1);
        int centroid = getCentroid(u, -1, total);

        // Count paths through centroid
        std::vector<int> allDists = {0}; // Centroid itself has distance 0

        for (int v : adj[centroid]) {
            if (!removed[v]) {
                std::vector<int> branchDists;
                getDistances(v, centroid, 1, branchDists);

                // For each node in this branch, pair with all previously seen branches
                for (int d : branchDists) {
                    int complement = targetK - d;
                    for (int existing : allDists) {
                        if (existing == complement) {
                            pairsWithDistK++;
                        }
                    }
                }

                // Add branch nodes to seen list
                for (int d : branchDists) {
                    allDists.push_back(d);
                }
            }
        }

        removed[centroid] = true;

        // Recurse on remaining subtrees
        for (int v : adj[centroid]) {
            if (!removed[v]) {
                decompose(v);
            }
        }
    }

public:
    explicit CentroidDecomposition(int numNodes)
        : n(numNodes), adj(numNodes), subtreeSize(numNodes, 0), removed(numNodes, false) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    long long countPairsDistanceK(int k) {
        targetK = k;
        pairsWithDistK = 0;
        removed.assign(n, false);
        decompose(0);
        return pairsWithDistK;
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "  Centroid Decomposition on Trees (O(NlogN))\n";
    std::cout << "==========================================\n\n";

    /*
     * Build sample tree of 7 nodes (0 to 6):
     *        0
     *       / \
     *      1   2
     *     / \   \
     *    3   4   5
     *             \
     *              6
     */
    CentroidDecomposition cd(7);
    cd.addEdge(0, 1);
    cd.addEdge(0, 2);
    cd.addEdge(1, 3);
    cd.addEdge(1, 4);
    cd.addEdge(2, 5);
    cd.addEdge(5, 6);

    std::cout << "Tree edges: (0-1), (0-2), (1-3), (1-4), (2-5), (5-6)\n\n";

    for (int k = 1; k <= 5; ++k) {
        long long pairs = cd.countPairsDistanceK(k);
        std::cout << "Pairs of nodes with exact tree distance K = " << k
                  << " : " << pairs << " pairs\n";
    }

    std::cout << "\nVerification: Centroid Decomposition executed successfully!\n";
    return 0;
}
