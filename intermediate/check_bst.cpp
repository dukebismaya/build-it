/*
 * Problem Description:
 * Check if a given Binary Tree is a Binary Search Tree (BST) or not.
 * We use an approach that keeps track of the min and max allowed values for each node.
 */

#include <iostream>
#include <climits>

struct Node {
    int data;
    Node* left;
    Node* right;
    Node(int val) : data(val), left(nullptr), right(nullptr) {}
};

bool isBSTUtil(Node* node, int min, int max) {
    if (node == nullptr)
        return true;
        
    if (node->data < min || node->data > max)
        return false;
        
    return isBSTUtil(node->left, min, node->data - 1) &&
           isBSTUtil(node->right, node->data + 1, max);
}

bool isBST(Node* node) {
    return isBSTUtil(node, INT_MIN, INT_MAX);
}

int main() {
    Node* root = new Node(4);
    root->left = new Node(2);
    root->right = new Node(5);
    root->left->left = new Node(1);
    root->left->right = new Node(3);

    if (isBST(root))
        std::cout << "The given tree is a BST.\n";
    else
        std::cout << "The given tree is NOT a BST.\n";
        
    return 0;
}
