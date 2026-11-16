/*
 * ============================================================================
 *  Cartesian Tree Builder (Implicit BST from Array)
 * ============================================================================
 *
 *  A Cartesian tree is a binary tree derived from an array where:
 *  - It satisfies the min-heap property: parent's value <= children's values.
 *  - An in-order traversal of the tree yields the original array.
 *
 *  This means the root is the minimum element, and recursively, the left
 *  subtree is the Cartesian tree of elements before the minimum, and the
 *  right subtree is the Cartesian tree of elements after.
 *
 *  Construction using a Stack (O(N)):
 *  - Process elements left to right.
 *  - Maintain a stack of nodes whose right spine is being built.
 *  - For each new element, pop nodes from the stack that are larger.
 *  - The last popped node becomes the left child of the new node.
 *  - The new node becomes the right child of the node remaining on top.
 *
 *  Applications:
 *  - Range Minimum Query (RMQ) reduction to LCA on Cartesian tree.
 *  - Treap structure (Cartesian tree on priorities).
 *  - Sorting-related problems and permutation analysis.
 *
 *  Time Complexity:  O(N) construction using stack
 *  Space Complexity: O(N)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <stack>
#include <cstdint>
#include <iomanip>
#include <algorithm>
#include <string>

namespace cartesian_tree {

struct Node {
    int value;
    int index;    /* Original array index */
    int left;     /* Left child index (-1 if none) */
    int right;    /* Right child index (-1 if none) */
    int parent;   /* Parent index (-1 if root) */
};

struct CartesianTree {
    std::vector<Node> nodes;
    int root;
};

/*
 * Build a min-Cartesian tree from an array in O(N).
 * Returns the tree structure with root index.
 */
CartesianTree build(const std::vector<int>& arr) {
    int n = static_cast<int>(arr.size());
    CartesianTree tree;
    tree.nodes.resize(n);
    tree.root = -1;

    if (n == 0) return tree;

    for (int i = 0; i < n; ++i) {
        tree.nodes[i] = {arr[i], i, -1, -1, -1};
    }

    std::stack<int> stk;

    for (int i = 0; i < n; ++i) {
        int lastPopped = -1;

        /* Pop all nodes with values greater than arr[i] */
        while (!stk.empty() && tree.nodes[stk.top()].value > arr[i]) {
            lastPopped = stk.top();
            stk.pop();
        }

        /* The last popped node becomes left child of current */
        if (lastPopped != -1) {
            tree.nodes[i].left = lastPopped;
            tree.nodes[lastPopped].parent = i;
        }

        /* Current becomes right child of stack top (if exists) */
        if (!stk.empty()) {
            tree.nodes[stk.top()].right = i;
            tree.nodes[i].parent = stk.top();
        }

        stk.push(i);
    }

    /* Root is the bottom of the stack */
    while (stk.size() > 1) stk.pop();
    tree.root = stk.top();

    return tree;
}

/* In-order traversal to verify it reproduces the original array */
void inOrder(const CartesianTree& tree, int node, std::vector<int>& result) {
    if (node == -1) return;
    inOrder(tree, tree.nodes[node].left, result);
    result.push_back(tree.nodes[node].value);
    inOrder(tree, tree.nodes[node].right, result);
}

/* Verify min-heap property */
bool verifyMinHeap(const CartesianTree& tree, int node) {
    if (node == -1) return true;

    int left = tree.nodes[node].left;
    int right = tree.nodes[node].right;

    if (left != -1 && tree.nodes[left].value < tree.nodes[node].value)
        return false;
    if (right != -1 && tree.nodes[right].value < tree.nodes[node].value)
        return false;

    return verifyMinHeap(tree, left) && verifyMinHeap(tree, right);
}

/* Pretty print the tree (rotated 90 degrees) */
void printTree(const CartesianTree& tree, int node, const std::string& prefix,
               bool isRight) {
    if (node == -1) return;

    std::cout << prefix;
    std::cout << (isRight ? "+-- " : "\\-- ");
    std::cout << tree.nodes[node].value << " (idx " << tree.nodes[node].index << ")\n";

    std::string newPrefix = prefix + (isRight ? "|   " : "    ");
    printTree(tree, tree.nodes[node].right, newPrefix, true);
    printTree(tree, tree.nodes[node].left, newPrefix, false);
}

/* Use Cartesian tree to answer RMQ: find minimum in arr[l..r] */
int rmqQuery(const CartesianTree& tree, int node, int l, int r) {
    if (node == -1) return INT32_MAX;
    if (tree.nodes[node].index < l) {
        return rmqQuery(tree, tree.nodes[node].right, l, r);
    }
    if (tree.nodes[node].index > r) {
        return rmqQuery(tree, tree.nodes[node].left, l, r);
    }
    /* node.index is in [l, r], and by heap property it's the minimum
       of its subtree restricted to [l, r] */
    return tree.nodes[node].value;
}

} // namespace cartesian_tree

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Cartesian Tree Builder (Implicit BST from Array)\n";
    std::cout << "========================================================\n\n";

    std::vector<int> arr = {3, 2, 6, 1, 9, 4, 5, 7};
    int n = static_cast<int>(arr.size());

    std::cout << "Input Array: [";
    for (int i = 0; i < n; ++i) {
        std::cout << arr[i] << (i + 1 < n ? ", " : "]\n\n");
    }

    auto tree = cartesian_tree::build(arr);

    std::cout << "Cartesian Tree Structure (root = "
              << tree.nodes[tree.root].value << " at index "
              << tree.root << "):\n\n";
    cartesian_tree::printTree(tree, tree.root, "  ", false);

    /* Verify in-order traversal */
    std::vector<int> inorderResult;
    cartesian_tree::inOrder(tree, tree.root, inorderResult);

    std::cout << "\nIn-order traversal: [";
    for (size_t i = 0; i < inorderResult.size(); ++i) {
        std::cout << inorderResult[i] << (i + 1 < inorderResult.size() ? ", " : "]\n");
    }
    std::cout << "Matches original array: "
              << (inorderResult == arr ? "YES" : "NO") << "\n";

    /* Verify min-heap property */
    std::cout << "Min-heap property: "
              << (cartesian_tree::verifyMinHeap(tree, tree.root) ? "SATISFIED" : "VIOLATED")
              << "\n";

    /* Node details */
    std::cout << "\nNode Details:\n";
    std::cout << std::setw(8) << "Index" << std::setw(8) << "Value"
              << std::setw(8) << "Left" << std::setw(8) << "Right"
              << std::setw(8) << "Parent" << "\n";
    std::cout << "----------------------------------------\n";

    for (int i = 0; i < n; ++i) {
        auto& nd = tree.nodes[i];
        std::cout << std::setw(8) << nd.index << std::setw(8) << nd.value
                  << std::setw(8) << nd.left << std::setw(8) << nd.right
                  << std::setw(8) << nd.parent << "\n";
    }

    /* RMQ queries using the Cartesian tree */
    std::cout << "\nRange Minimum Queries via Cartesian Tree:\n";
    struct RMQ { int l, r; };
    std::vector<RMQ> queries = {{0, 7}, {0, 2}, {4, 7}, {2, 5}, {1, 3}};

    for (auto& q : queries) {
        int result = cartesian_tree::rmqQuery(tree, tree.root, q.l, q.r);
        int brute = *std::min_element(arr.begin() + q.l, arr.begin() + q.r + 1);
        std::cout << "  min[" << q.l << "," << q.r << "] = " << result
                  << " (expected " << brute << ") "
                  << (result == brute ? "OK" : "MISMATCH") << "\n";
    }

    return 0;
}
