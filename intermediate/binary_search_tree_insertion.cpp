/*
 * Problem Description:
 * Implement a Binary Search Tree (BST) and write a function to insert new nodes.
 * Provide a method to display the tree using in-order traversal to verify sorting.
 */

#include <iostream>

struct Node {
    int key;
    Node* left;
    Node* right;
    Node(int val) : key(val), left(nullptr), right(nullptr) {}
};

class BST {
private:
    Node* root;

    Node* insertRec(Node* node, int key) {
        if (node == nullptr)
            return new Node(key);
            
        if (key < node->key)
            node->left = insertRec(node->left, key);
        else if (key > node->key)
            node->right = insertRec(node->right, key);
            
        return node;
    }

    void inorderRec(Node* root) {
        if (root != nullptr) {
            inorderRec(root->left);
            std::cout << root->key << " ";
            inorderRec(root->right);
        }
    }

public:
    BST() : root(nullptr) {}

    void insert(int key) {
        root = insertRec(root, key);
    }

    void inorder() {
        inorderRec(root);
        std::cout << "\n";
    }
};

int main() {
    BST tree;
    tree.insert(50);
    tree.insert(30);
    tree.insert(20);
    tree.insert(40);
    tree.insert(70);
    tree.insert(60);
    tree.insert(80);

    std::cout << "Inorder traversal of the given tree \n";
    tree.inorder();

    return 0;
}
