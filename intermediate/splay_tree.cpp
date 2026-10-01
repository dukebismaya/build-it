/*
 * Problem Description:
 * Implement Splay Tree concept.
 * A splay tree is a self-adjusting binary search tree with the additional property 
 * that recently accessed elements are quick to access again.
 */

#include <iostream>
using namespace std;

class Node {
public:
    int key;
    Node* left;
    Node* right;
    
    Node(int k) {
        key = k;
        left = right = nullptr;
    }
};

Node* rightRotate(Node* x) {
    Node* y = x->left;
    x->left = y->right;
    y->right = x;
    return y;
}

Node* leftRotate(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

Node* splay(Node* root, int key) {
    if (root == nullptr || root->key == key)
        return root;
        
    if (root->key > key) {
        if (root->left == nullptr) return root;
        
        if (root->left->key > key) {
            root->left->left = splay(root->left->left, key);
            root = rightRotate(root);
        }
        else if (root->left->key < key) {
            root->left->right = splay(root->left->right, key);
            if (root->left->right != nullptr)
                root->left = leftRotate(root->left);
        }
        return (root->left == nullptr) ? root : rightRotate(root);
    }
    else {
        if (root->right == nullptr) return root;
        
        if (root->right->key > key) {
            root->right->left = splay(root->right->left, key);
            if (root->right->left != nullptr)
                root->right = rightRotate(root->right);
        }
        else if (root->right->key < key) {
            root->right->right = splay(root->right->right, key);
            root = leftRotate(root);
        }
        return (root->right == nullptr) ? root : leftRotate(root);
    }
}

Node* search(Node* root, int key) {
    return splay(root, key);
}

void preOrder(Node* root) {
    if (root != nullptr) {
        cout << root->key << " ";
        preOrder(root->left);
        preOrder(root->right);
    }
}

int main() {
    Node* root = new Node(100);
    root->left = new Node(50);
    root->right = new Node(200);
    root->left->left = new Node(40);
    root->left->left->left = new Node(30);
    root->left->left->left->left = new Node(20);
    
    root = search(root, 20);
    cout << "Preorder traversal of the modified Splay tree is \n";
    preOrder(root);
    return 0;
}
