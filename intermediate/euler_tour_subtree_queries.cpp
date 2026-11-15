/*
 * ============================================================================
 *  Euler Tour Tree — Subtree Queries via Flattening
 * ============================================================================
 *
 *  An Euler Tour of a rooted tree visits each node twice (on entry and exit),
 *  producing a linear array of size 2N. This flattening converts subtree
 *  queries into contiguous range queries on the array, enabling the use of
 *  standard data structures (prefix sums, segment trees, BITs).
 *
 *  Properties:
 *  - Subtree of node v occupies indices [tin[v], tout[v]] in the tour.
 *  - LCA queries can be answered using the Euler Tour + RMQ.
 *  - Subtree aggregate queries become range queries on the flat array.
 *
 *  This implementation demonstrates:
 *  1) Building the Euler Tour (DFS order) with in/out timestamps.
 *  2) Answering subtree sum queries using prefix sums on the tour.
 *  3) Checking ancestor relationships in O(1) using timestamps.
 *
 *  Time Complexity:
 *  - Tour construction: O(N)
 *  - Subtree sum query: O(1) with prefix sums
 *  - Ancestor check: O(1)
 *  Space Complexity: O(N)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

namespace euler_tour {

class EulerTourTree {
public:
    EulerTourTree(int n, int root = 0)
        : numNodes(n), rootNode(root), adj(n), weight(n, 0),
          tin(n, -1), tout(n, -1), depth(n, 0), parent(n, -1),
          subtreeSize(n, 0), timer(0) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void setWeight(int node, int64_t w) {
        weight[node] = w;
    }

    void build() {
        timer = 0;
        tourOrder.clear();
        tourOrder.reserve(numNodes);
        dfs(rootNode, -1, 0);

        /* Build prefix sums for subtree sum queries */
        prefixSum.resize(numNodes + 1, 0);
        for (int i = 0; i < numNodes; ++i) {
            prefixSum[i + 1] = prefixSum[i] + weight[tourOrder[i]];
        }
    }

    /* Subtree sum for node v: sum of weights in subtree */
    int64_t subtreeSum(int v) const {
        return prefixSum[tout[v] + 1] - prefixSum[tin[v]];
    }

    /* Check if u is an ancestor of v */
    bool isAncestor(int u, int v) const {
        return tin[u] <= tin[v] && tout[v] <= tout[u];
    }

    int getDepth(int v) const { return depth[v]; }
    int getParent(int v) const { return parent[v]; }
    int getSubtreeSize(int v) const { return subtreeSize[v]; }
    int getTin(int v) const { return tin[v]; }
    int getTout(int v) const { return tout[v]; }

    const std::vector<int>& getTourOrder() const { return tourOrder; }

private:
    int numNodes;
    int rootNode;
    std::vector<std::vector<int>> adj;
    std::vector<int64_t> weight;
    std::vector<int> tin, tout, depth, parent;
    std::vector<int> subtreeSize;
    std::vector<int> tourOrder;
    std::vector<int64_t> prefixSum;
    int timer;

    void dfs(int v, int par, int dep) {
        parent[v] = par;
        depth[v] = dep;
        tin[v] = timer++;
        tourOrder.push_back(v);
        subtreeSize[v] = 1;

        for (int u : adj[v]) {
            if (u != par) {
                dfs(u, v, dep + 1);
                subtreeSize[v] += subtreeSize[u];
            }
        }

        tout[v] = timer - 1;
    }
};

} // namespace euler_tour

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Euler Tour Tree — Subtree Queries via DFS Flattening\n";
    std::cout << "========================================================\n\n";

    /*
     * Tree structure (rooted at 0):
     *
     *          0 (w=10)
     *         / \
     *        1   2 (w=5, w=8)
     *       /|    \
     *      3 4     5 (w=3, w=7, w=2)
     *     /       / \
     *    6       7   8 (w=1, w=4, w=6)
     */

    int n = 9;
    euler_tour::EulerTourTree tree(n, 0);

    tree.addEdge(0, 1);
    tree.addEdge(0, 2);
    tree.addEdge(1, 3);
    tree.addEdge(1, 4);
    tree.addEdge(2, 5);
    tree.addEdge(3, 6);
    tree.addEdge(5, 7);
    tree.addEdge(5, 8);

    int64_t weights[] = {10, 5, 8, 3, 7, 2, 1, 4, 6};
    for (int i = 0; i < n; ++i) {
        tree.setWeight(i, weights[i]);
    }

    tree.build();

    /* Display Euler Tour */
    std::cout << "Euler Tour (DFS order): [";
    const auto& tour = tree.getTourOrder();
    for (size_t i = 0; i < tour.size(); ++i) {
        std::cout << tour[i] << (i + 1 < tour.size() ? ", " : "]\n\n");
    }

    /* Node information */
    std::cout << "Node Information:\n";
    std::cout << std::setw(6) << "Node" << std::setw(8) << "Weight"
              << std::setw(8) << "Depth" << std::setw(8) << "Parent"
              << std::setw(8) << "tin" << std::setw(8) << "tout"
              << std::setw(10) << "SubSize" << "\n";
    std::cout << "--------------------------------------------------------\n";

    for (int i = 0; i < n; ++i) {
        std::cout << std::setw(6) << i
                  << std::setw(8) << weights[i]
                  << std::setw(8) << tree.getDepth(i)
                  << std::setw(8) << tree.getParent(i)
                  << std::setw(8) << tree.getTin(i)
                  << std::setw(8) << tree.getTout(i)
                  << std::setw(10) << tree.getSubtreeSize(i) << "\n";
    }

    /* Subtree sum queries */
    std::cout << "\nSubtree Sum Queries:\n";
    std::cout << "--------------------------------------------\n";

    for (int v = 0; v < n; ++v) {
        std::cout << "  subtreeSum(" << v << ") = " << tree.subtreeSum(v) << "\n";
    }

    /* Verify: subtreeSum(0) should be total of all weights */
    int64_t total = 0;
    for (int i = 0; i < n; ++i) total += weights[i];
    std::cout << "\n  Total weight (all nodes) = " << total << "\n";
    std::cout << "  subtreeSum(root=0) matches: "
              << (tree.subtreeSum(0) == total ? "YES" : "NO") << "\n";

    /* Ancestor checks */
    std::cout << "\nAncestor Relationship Checks (O(1) each):\n";
    std::cout << "--------------------------------------------\n";

    struct AncQuery { int u; int v; };
    std::vector<AncQuery> ancQueries = {
        {0, 6}, {1, 6}, {2, 6}, {5, 7}, {3, 4}, {0, 0}
    };

    for (auto& aq : ancQueries) {
        std::cout << "  isAncestor(" << aq.u << ", " << aq.v << ") = "
                  << (tree.isAncestor(aq.u, aq.v) ? "true" : "false") << "\n";
    }

    return 0;
}
