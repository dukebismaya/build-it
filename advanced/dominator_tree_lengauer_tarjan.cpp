/**
 * @file dominator_tree_lengauer_tarjan.cpp
 * @brief Computes the Dominator Tree of a directed flowgraph using the Lengauer-Tarjan algorithm.
 *
 * Concepts:
 * - In a directed graph with an entry node r, node u dominates v (u dom v) if every path
 *   from r to v must pass through u.
 * - The immediate dominator idom(v) is the unique strict dominator that is closest to v.
 * - The dominance relation forms a tree rooted at r (the Dominator Tree).
 * - Dominator trees are foundational in optimizing compilers (LLVM, GCC) for constructing
 *   Static Single Assignment (SSA) form, finding dominance frontiers, and identifying natural loops.
 *
 * Lengauer-Tarjan Algorithm:
 * 1. Perform DFS from root to assign DFS numbers (dfn) to vertices.
 * 2. Compute semidominators (sdom) for vertices in reverse DFS order.
 * 3. Use Disjoint Set Union (DSU) with path compression (eval/link) to resolve idom.
 * 4. Adjust idom values in forward DFS order.
 *
 * Time Complexity: O(E * alpha(V)) where alpha is the inverse Ackermann function.
 * Space Complexity: O(V + E)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

class DominatorTree {
private:
    int n;
    int root;
    std::vector<std::vector<int>> adj;     // forward edges
    std::vector<std::vector<int>> revAdj;  // reverse edges
    std::vector<std::vector<int>> bucket;  // vertices with same sdom

    // Lengauer-Tarjan state
    std::vector<int> dfn;      // DFS discovery time (1-indexed)
    std::vector<int> vertex;   // vertex[dfn] = original vertex id
    std::vector<int> parent;   // parent in DFS tree
    std::vector<int> sdom;     // semidominator
    std::vector<int> idom;     // immediate dominator
    std::vector<int> dsu;      // DSU parent
    std::vector<int> mn;       // minimum sdom on path to root in DSU
    int timer;

    void dfs(int u) {
        timer++;
        dfn[u] = timer;
        vertex[timer] = u;
        sdom[u] = u;

        for (int v : adj[u]) {
            if (dfn[v] == 0) {
                parent[v] = u;
                dfs(v);
            }
            revAdj[v].push_back(u);
        }
    }

    int find(int u) {
        if (dsu[u] == u) return u;
        int rootNode = find(dsu[u]);
        if (dfn[sdom[mn[dsu[u]]]] < dfn[sdom[mn[u]]]) {
            mn[u] = mn[dsu[u]];
        }
        return dsu[u] = rootNode;
    }

    int eval(int u) {
        find(u);
        return mn[u];
    }

public:
    DominatorTree(int numVertices, int entryRoot)
        : n(numVertices),
          root(entryRoot),
          adj(numVertices),
          revAdj(numVertices),
          bucket(numVertices),
          dfn(numVertices, 0),
          vertex(numVertices + 1, 0),
          parent(numVertices, -1),
          sdom(numVertices, -1),
          idom(numVertices, -1),
          dsu(numVertices),
          mn(numVertices),
          timer(0) {
        for (int i = 0; i < n; ++i) {
            dsu[i] = i;
            mn[i] = i;
        }
    }

    void addEdge(int u, int v) {
        if (u >= 0 && u < n && v >= 0 && v < n) {
            adj[u].push_back(v);
        }
    }

    void computeDominatorTree() {
        // Step 1: DFS spanning tree
        dfs(root);

        // Step 2 & 3: Compute semidominators & intermediate immediate dominators in reverse DFS order
        for (int i = timer; i >= 2; --i) {
            int w = vertex[i];

            // Compute semidominator of w
            for (int v : revAdj[w]) {
                if (dfn[v] == 0) continue; // unreachable node
                int u = eval(v);
                if (dfn[sdom[u]] < dfn[sdom[w]]) {
                    sdom[w] = sdom[u];
                }
            }

            bucket[sdom[w]].push_back(w);
            dsu[w] = parent[w];

            // Empty the bucket of parent[w]
            for (int v : bucket[parent[w]]) {
                int u = eval(v);
                if (sdom[u] == sdom[v]) {
                    idom[v] = sdom[v];
                } else {
                    idom[v] = u; // to be resolved in step 4
                }
            }
            bucket[parent[w]].clear();
        }

        // Step 4: Finalize immediate dominators in forward DFS order
        for (int i = 2; i <= timer; ++i) {
            int w = vertex[i];
            if (idom[w] != sdom[w]) {
                idom[w] = idom[idom[w]];
            }
        }
        idom[root] = root;
    }

    int getIdom(int u) const {
        return idom[u];
    }

    std::vector<std::vector<int>> getDominatorTreeAdj() const {
        std::vector<std::vector<int>> tree(n);
        for (int i = 0; i < n; ++i) {
            if (i != root && idom[i] != -1) {
                tree[idom[i]].push_back(i);
            }
        }
        return tree;
    }

    void printDominators() const {
        std::cout << "  " << std::setw(8) << "Node"
                  << std::setw(12) << "DFS Num"
                  << std::setw(12) << "Semidom"
                  << std::setw(12) << "Idom" << '\n';
        std::cout << "  " << std::string(44, '-') << '\n';

        for (int i = 0; i < n; ++i) {
            std::cout << "  " << std::setw(8) << i;
            if (dfn[i] == 0) {
                std::cout << std::setw(12) << "Unreachable"
                          << std::setw(12) << "-"
                          << std::setw(12) << "-" << '\n';
            } else {
                std::cout << std::setw(12) << dfn[i]
                          << std::setw(12) << sdom[i]
                          << std::setw(12) << idom[i] << '\n';
            }
        }
    }
};

int main() {
    std::cout << "=== Lengauer-Tarjan Dominator Tree Algorithm ===\n\n";

    /*
     * Classic Flowgraph Example:
     * Node 0 is Root.
     * Edges:
     *   0 -> 1, 0 -> 2
     *   1 -> 3
     *   2 -> 3, 2 -> 4
     *   3 -> 5
     *   4 -> 5
     *   5 -> 1 (back-edge)
     */
    DominatorTree dt(6, 0);
    dt.addEdge(0, 1);
    dt.addEdge(0, 2);
    dt.addEdge(1, 3);
    dt.addEdge(2, 3);
    dt.addEdge(2, 4);
    dt.addEdge(3, 5);
    dt.addEdge(4, 5);
    dt.addEdge(5, 1);

    std::cout << "Analyzing Control Flow Graph (6 nodes, Entry = 0):\n";
    dt.computeDominatorTree();
    dt.printDominators();

    std::cout << "\nDominator Tree Children:\n";
    auto domTree = dt.getDominatorTreeAdj();
    for (size_t u = 0; u < domTree.size(); ++u) {
        if (!domTree[u].empty()) {
            std::cout << "  Node " << u << " directly dominates: ";
            for (int v : domTree[u]) {
                std::cout << v << " ";
            }
            std::cout << '\n';
        }
    }

    return 0;
}
