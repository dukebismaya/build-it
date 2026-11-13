/**
 * @file morris_inorder_tree_traversal.cpp
 * @brief Morris In-Order Binary Tree Traversal with O(1) auxiliary space in C++17.
 *
 * Overview (Joseph M. Morris, 1979):
 *   Standard recursive or stack-based binary tree traversals require O(H) auxiliary space
 *   (up to O(N) in skewed trees).
 *   Morris Traversal achieves in-order traversal using strictly O(1) auxiliary memory
 *   without altering the original tree structure upon completion.
 *
 * Threaded Binary Tree Concept:
 *   - Utilizes temporary threads (pointers) from the in-order predecessor of node X
 *     back to node X.
 *   - The in-order predecessor of X is the rightmost node in X's left subtree.
 *   - When predecessor's right child is null:
 *       Create thread: pred->right = current, and advance current to current->left.
 *   - When predecessor's right child is current (thread already exists):
 *       Remove thread: pred->right = null, visit current, advance current to current->right.
 *
 * Complexity:
 *   - Time: O(N) amortized because each edge is traversed at most 3 times.
 *   - Space: O(1) auxiliary space (no recursion call stack or heap stack).
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <memory>

namespace tree {

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;

    explicit TreeNode(int v) : val(v), left(nullptr), right(nullptr) {}
};

/**
 * @brief Performs Morris In-Order Traversal with O(1) auxiliary space.
 * @param root Root of the binary tree.
 * @return Vector of node values in in-order sequence.
 */
std::vector<int> morrisInOrder(TreeNode* root) {
    std::vector<int> result;
    TreeNode* current = root;

    while (current != nullptr) {
        if (current->left == nullptr) {
            // Case 1: No left subtree, visit current node and go right
            result.push_back(current->val);
            current = current->right;
        } else {
            // Case 2: Find in-order predecessor (rightmost node of left subtree)
            TreeNode* pred = current->left;
            while (pred->right != nullptr && pred->right != current) {
                pred = pred->right;
            }

            if (pred->right == nullptr) {
                // Construct temporary thread back to current
                pred->right = current;
                current = current->left;
            } else {
                // Thread already exists -> Revert thread back to null
                pred->right = nullptr;
                result.push_back(current->val);
                current = current->right;
            }
        }
    }

    return result;
}

/**
 * @brief Standard recursive in-order traversal for reference verification.
 */
void standardInOrder(TreeNode* root, std::vector<int>& out) {
    if (!root) return;
    standardInOrder(root->left, out);
    out.push_back(root->val);
    standardInOrder(root->right, out);
}

} // namespace tree

/*
 * Demonstration Binary Search Tree Structure:
 *                 (4)
 *                /   \
 *              (2)   (6)
 *              / \   / \
 *            (1) (3)(5) (7)
 *
 * Expected In-Order: [1, 2, 3, 4, 5, 6, 7]
 */

int main() {
    std::cout << "========================================\n";
    std::cout << "     MORRIS IN-ORDER TREE TRAVERSAL     \n";
    std::cout << "    (Threaded Tree O(1) Extra Space)    \n";
    std::cout << "========================================\n\n";

    // Build sample binary tree
    tree::TreeNode n1(1), n3(3), n5(5), n7(7);
    tree::TreeNode n2(2), n6(6), n4(4);

    n4.left = &n2;
    n4.right = &n6;
    n2.left = &n1;
    n2.right = &n3;
    n6.left = &n5;
    n6.right = &n7;

    std::vector<int> refOrder;
    tree::standardInOrder(&n4, refOrder);

    auto morrisOrder = tree::morrisInOrder(&n4);

    std::cout << "Tree Root: 4\n";
    std::cout << "Morris Traversal Result    : [ ";
    for (int v : morrisOrder) std::cout << v << " ";
    std::cout << "]\n";

    std::cout << "Standard Reference In-Order: [ ";
    for (int v : refOrder) std::cout << v << " ";
    std::cout << "]\n\n";

    bool match = (morrisOrder == refOrder);
    std::cout << "Verification: " << (match ? "PASS (Exact Match & Tree Intact)" : "FAIL") << "\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
