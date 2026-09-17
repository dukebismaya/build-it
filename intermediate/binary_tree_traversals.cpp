/*
 * Problem Description:
 * Implement a Binary Tree and perform its Traversals: Inorder, Preorder, and Postorder.
 */

#include <iostream>

struct Node {
    int data;
    struct Node *left, *right;
    Node(int data) {
        this->data = data;
        left = right = nullptr;
    }
};

void printPostorder(struct Node* node) {
    if (node == nullptr)
        return;
    printPostorder(node->left);
    printPostorder(node->right);
    std::cout << node->data << " ";
}

void printInorder(struct Node* node) {
    if (node == nullptr)
        return;
    printInorder(node->left);
    std::cout << node->data << " ";
    printInorder(node->right);
}

void printPreorder(struct Node* node) {
    if (node == nullptr)
        return;
    std::cout << node->data << " ";
    printPreorder(node->left);
    printPreorder(node->right);
}

int main() {
    struct Node* root = new Node(1);
    root->left = new Node(2);
    root->right = new Node(3);
    root->left->left = new Node(4);
    root->left->right = new Node(5);

    std::cout << "\nPreorder traversal: ";
    printPreorder(root);

    std::cout << "\nInorder traversal: ";
    printInorder(root);

    std::cout << "\nPostorder traversal: ";
    printPostorder(root);
    
    std::cout << "\n";
    return 0;
}
