/**
 * @file cartesian_tree_construction.cpp
 * @brief Linear-time O(N) Cartesian Tree construction using a monotonic stack in C++17.
 *
 * Definition:
 *   A Cartesian Tree for an array A[0..N-1] of distinct values is a binary tree with:
 *     1. In-order traversal property: In-order traversal of the tree visits nodes
 *        in the exact order of the original array (0, 1, ..., N-1).
 *     2. Min-Heap property: The key at any node is strictly smaller than the keys
 *        at its left and right children (for a Min-Cartesian Tree).
 *
 * Linear Time Construction Algorithm:
 *   - Maintain the "right spine" (path from root to the rightmost node) using an explicit stack.
 *   - For each element A[i] (i = 0 to N-1):
 *       1. Pop nodes from the stack while their value is greater than A[i].
 *       2. The LAST popped node becomes the left child of node i.
 *       3. If the stack is not empty after popping, node i becomes the right child of the stack top.
 *       4. Push node i onto the stack.
 *   - The bottom of the stack (or the first element pushed that was never popped) is the root.
 *
 * Fundamental Application:
 *   - Range Minimum Query (RMQ) on A[L..R] corresponds exactly to the Lowest Common
 *     Ancestor (LCA) of nodes L and R in the Cartesian tree.
 *
 * Complexity:
 *   - Time: O(N) because each node is pushed onto the stack once and popped at most once.
 *   - Auxiliary Space: O(N) for tree pointers and stack.
 */

#include <iostream>
#include <vector>
#include <stack>
#include <iomanip>
#include <string>

namespace tree {

struct CartesianNode {
    int index;
    int value;
    int left;
    int right;
    int parent;

    CartesianNode(int idx = -1, int val = 0)
        : index(idx), value(val), left(-1), right(-1), parent(-1) {}
};

class CartesianTree {
private:
    std::vector<CartesianNode> nodes;
    int root;

public:
    explicit CartesianTree(const std::vector<int>& arr) : root(-1) {
        int n = static_cast<int>(arr.size());
        if (n == 0) return;

        nodes.resize(n);
        for (int i = 0; i < n; ++i) {
            nodes[i] = CartesianNode(i, arr[i]);
        }

        std::vector<int> st; // Stack stores node indices along the right spine

        for (int i = 0; i < n; ++i) {
            int lastPopped = -1;
            while (!st.empty() && nodes[st.back()].value > arr[i]) {
                lastPopped = st.back();
                st.pop_back();
            }

            if (lastPopped != -1) {
                nodes[i].left = lastPopped;
                nodes[lastPopped].parent = i;
            }

            if (!st.empty()) {
                nodes[st.back()].right = i;
                nodes[i].parent = st.back();
            }

            st.push_back(i);
        }

        // The bottom-most element of the stack is the root of the entire tree
        root = st.front();
    }

    int getRoot() const { return root; }
    const std::vector<CartesianNode>& getNodes() const { return nodes; }

    void inOrderTraversal(int u, std::vector<int>& result) const {
        if (u == -1) return;
        inOrderTraversal(nodes[u].left, result);
        result.push_back(nodes[u].value);
        inOrderTraversal(nodes[u].right, result);
    }

    bool verifyMinHeap(int u) const {
        if (u == -1) return true;
        if (nodes[u].left != -1 && nodes[nodes[u].left].value < nodes[u].value) return false;
        if (nodes[u].right != -1 && nodes[nodes[u].right].value < nodes[u].value) return false;
        return verifyMinHeap(nodes[u].left) && verifyMinHeap(nodes[u].right);
    }
};

} // namespace tree

/*
 * Demonstration Array:
 * Index:  0   1   2   3   4   5   6
 * Value: [9,  3,  7,  1,  8, 12, 10]
 *
 * Min-Cartesian Tree structure:
 *                 (1 @ idx 3)
 *                /           \
 *          (3 @ idx 1)     (8 @ idx 4)
 *          /        \                 \
 *     (9 @ 0)     (7 @ 2)          (10 @ idx 6)
 *                                  /
 *                             (12 @ idx 5)
 */

int main() {
    std::cout << "========================================\n";
    std::cout << "  CARTESIAN TREE O(N) STACK CONSTRUCTION \n";
    std::cout << "========================================\n\n";

    std::vector<int> arr = {9, 3, 7, 1, 8, 12, 10};

    std::cout << "Input Array: [ ";
    for (int v : arr) std::cout << v << " ";
    std::cout << "]\n\n";

    tree::CartesianTree ct(arr);
    const auto& nodes = ct.getNodes();

    std::cout << "Root Node: Index " << ct.getRoot() << " (Value = " << arr[ct.getRoot()] << ")\n\n";

    std::cout << std::left << std::setw(8) << "Index"
              << std::setw(10) << "Value"
              << std::setw(12) << "Left Child"
              << std::setw(14) << "Right Child"
              << "Parent\n";
    std::cout << std::string(54, '-') << "\n";

    for (size_t i = 0; i < nodes.size(); ++i) {
        std::string leftStr = (nodes[i].left == -1) ? "None" : std::to_string(nodes[i].left) + " (" + std::to_string(arr[nodes[i].left]) + ")";
        std::string rightStr = (nodes[i].right == -1) ? "None" : std::to_string(nodes[i].right) + " (" + std::to_string(arr[nodes[i].right]) + ")";
        std::string parentStr = (nodes[i].parent == -1) ? "ROOT" : std::to_string(nodes[i].parent) + " (" + std::to_string(arr[nodes[i].parent]) + ")";

        std::cout << std::left << std::setw(8) << i
                  << std::setw(10) << nodes[i].value
                  << std::setw(12) << leftStr
                  << std::setw(14) << rightStr
                  << parentStr << "\n";
    }

    std::vector<int> inOrder;
    ct.inOrderTraversal(ct.getRoot(), inOrder);

    std::cout << "\nIn-Order Traversal Check: [ ";
    for (int v : inOrder) std::cout << v << " ";
    std::cout << "]\n";

    bool matchesOriginal = (inOrder == arr);
    bool heapValid = ct.verifyMinHeap(ct.getRoot());

    std::cout << "  - Matches Original Sequence? " << (matchesOriginal ? "YES" : "NO") << "\n";
    std::cout << "  - Satisfies Min-Heap Property? " << (heapValid ? "YES" : "NO") << "\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
