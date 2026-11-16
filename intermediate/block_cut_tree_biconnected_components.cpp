/**
 * ============================================================================
 * Block-Cut Tree (Biconnected Components Condensation)
 * ============================================================================
 * 
 * Description:
 * The Block-Cut Tree (BCT) decomposes any connected undirected graph into a
 * bipartite tree representing its 2-vertex-connected components (blocks) and
 * articulation points (cut vertices).
 * 
 * Structure of the Block-Cut Tree:
 * - Square Nodes (B): Represent maximal 2-vertex-connected subgraphs (blocks).
 * - Circle Nodes (C): Represent articulation points (cut vertices).
 * - An undirected edge exists between block B and cut vertex C if and only if
 *   C belongs to block B.
 * 
 * ASCII Visualization:
 *    Original Graph:                Block-Cut Tree:
 *      (0)---(1)                       [B0: {0,1,2}]
 *       |  /   |                             |
 *      (2)    (3)---(4)                    (1: cut)
 *              |  /                          |
 *             (5)                      [B1: {1,3}]
 *                                            |
 *                                          (3: cut)
 *                                            |
 *                                      [B2: {3,4,5}]
 * 
 * Applications:
 * - Simple path bottlenecks: Any simple path between vertices u and v must
 *   pass through all cut vertices on the unique tree path between their blocks.
 * - Dynamic vertex removal resilience.
 * - 2-vertex-connectivity analysis.
 * 
 * Time Complexity:
 * - O(V + E) for Tarjan's biconnected component decomposition and tree construction
 * - Space Complexity: O(V + E)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <cstdint>
#include <iomanip>

namespace block_cut_tree {

struct Edge {
    int u, v;
};

class BlockCutTreeBuilder {
public:
    int n;
    std::vector<std::vector<int>> adj;

    // Output structures
    std::vector<bool> is_cut;
    std::vector<std::vector<int>> blocks; // List of vertices in each block
    int bct_size;
    std::vector<std::vector<int>> bct_adj; // Block-cut tree adjacency list
    std::vector<int> node_to_bct; // Maps original vertex to its BCT node

    explicit BlockCutTreeBuilder(int num_vertices)
        : n(num_vertices), adj(num_vertices), is_cut(num_vertices, false),
          bct_size(0), node_to_bct(num_vertices, -1) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void build() {
        tin.assign(n, -1);
        low.assign(n, -1);
        timer = 0;
        stk.clear();
        blocks.clear();
        is_cut.assign(n, false);

        for (int i = 0; i < n; ++i) {
            if (tin[i] == -1) {
                dfs(i, -1);
            }
        }

        // Assign IDs in BCT:
        // Cut vertices keep IDs 0 .. n-1 (if is_cut[v] == true)
        // Blocks are assigned IDs starting from n
        int num_blocks = static_cast<int>(blocks.size());
        bct_size = n + num_blocks;
        bct_adj.assign(bct_size, std::vector<int>());

        for (int b_idx = 0; b_idx < num_blocks; ++b_idx) {
            int block_node = n + b_idx;
            for (int v : blocks[b_idx]) {
                if (is_cut[v]) {
                    bct_adj[v].push_back(block_node);
                    bct_adj[block_node].push_back(v);
                } else {
                    node_to_bct[v] = block_node;
                }
            }
        }

        for (int v = 0; v < n; ++v) {
            if (is_cut[v]) {
                node_to_bct[v] = v;
            }
        }
    }

private:
    std::vector<int> tin, low;
    int timer;
    std::vector<int> stk;

    void dfs(int u, int p) {
        tin[u] = low[u] = ++timer;
        stk.push_back(u);
        int children = 0;

        for (int to : adj[u]) {
            if (to == p) continue;
            if (tin[to] != -1) {
                low[u] = std::min(low[u], tin[to]);
            } else {
                children++;
                dfs(to, u);
                low[u] = std::min(low[u], low[to]);

                if (low[to] >= tin[u]) {
                    if (p != -1 || children > 1) {
                        is_cut[u] = true;
                    }

                    // Extract new block
                    std::vector<int> block;
                    while (true) {
                        int v = stk.back();
                        stk.pop_back();
                        block.push_back(v);
                        if (v == to) break;
                    }
                    block.push_back(u);
                    blocks.push_back(block);
                }
            }
        }
    }
};

} // namespace block_cut_tree

int main() {
    std::cout << "========================================================\n";
    std::cout << " Block-Cut Tree (Biconnected Components Condensation)\n";
    std::cout << "========================================================\n\n";

    // Build a graph with 7 vertices:
    // Triangle: 0-1, 1-2, 2-0
    // Edge: 1-3
    // Triangle: 3-4, 4-5, 5-3
    // Pendant edge: 5-6
    int n = 7;
    block_cut_tree::BlockCutTreeBuilder builder(n);

    builder.addEdge(0, 1);
    builder.addEdge(1, 2);
    builder.addEdge(2, 0);

    builder.addEdge(1, 3);

    builder.addEdge(3, 4);
    builder.addEdge(4, 5);
    builder.addEdge(5, 3);

    builder.addEdge(5, 6);

    builder.build();

    std::cout << "Graph structure with " << n << " vertices and 8 edges analyzed.\n\n";

    std::cout << "1. Articulation Points (Cut Vertices):\n";
    for (int i = 0; i < n; ++i) {
        if (builder.is_cut[i]) {
            std::cout << "   - Vertex " << i << " is a CUT VERTEX\n";
        }
    }

    std::cout << "\n2. Biconnected Components (Blocks):\n";
    for (size_t b = 0; b < builder.blocks.size(); ++b) {
        std::cout << "   - Block " << b << " (BCT node " << (n + b) << "): {";
        for (size_t i = 0; i < builder.blocks[b].size(); ++i) {
            std::cout << builder.blocks[b][i] << (i + 1 < builder.blocks[b].size() ? ", " : "}\n");
        }
    }

    std::cout << "\n3. Block-Cut Tree Adjacency Structure:\n";
    for (int i = 0; i < builder.bct_size; ++i) {
        if (!builder.bct_adj[i].empty()) {
            if (i < n) {
                std::cout << "   Cut Vertex (" << i << ") connects to: ";
            } else {
                std::cout << "   Block [" << (i - n) << "] connects to: ";
            }
            for (size_t j = 0; j < builder.bct_adj[i].size(); ++j) {
                int neighbor = builder.bct_adj[i][j];
                if (neighbor < n) {
                    std::cout << "Cut(" << neighbor << ") ";
                } else {
                    std::cout << "Block[" << (neighbor - n) << "] ";
                }
            }
            std::cout << "\n";
        }
    }

    std::cout << "\nBlock-Cut Tree successfully constructed in O(V + E)!\n";
    return 0;
}
