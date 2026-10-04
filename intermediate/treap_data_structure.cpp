/*
 * Problem Description:
 * Implement a Treap (Tree + Heap) conceptually.
 * A treap is a randomized binary search tree that maintains both BST properties 
 * for keys and heap properties for priorities.
 */

#include <iostream>
#include <cstdlib>
using namespace std;

class TreapNode {
public:
    int key, priority;
    TreapNode *left, *right;

    TreapNode(int k) {
        key = k;
        priority = rand() % 100;
        left = right = nullptr;
    }
};

TreapNode* rightRotate(TreapNode* y) {
    TreapNode* x = y->left;
    TreapNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    return x;
}

TreapNode* leftRotate(TreapNode* x) {
    TreapNode* y = x->right;
    TreapNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    return y;
}

TreapNode* insert(TreapNode* root, int key) {
    if (!root)
        return new TreapNode(key);

    if (key <= root->key) {
        root->left = insert(root->left, key);
        if (root->left->priority > root->priority)
            root = rightRotate(root);
    } else {
        root->right = insert(root->right, key);
        if (root->right->priority > root->priority)
            root = leftRotate(root);
    }
    return root;
}

void inorder(TreapNode* root) {
    if (root) {
        inorder(root->left);
        cout << "key: " << root->key << " | priority: " << root->priority;
        if (root->left) cout << " | left child: " << root->left->key;
        if (root->right) cout << " | right child: " << root->right->key;
        cout << endl;
        inorder(root->right);
    }
}

int main() {
    srand(1); // Seed for deterministic output in testing
    TreapNode* root = nullptr;
    root = insert(root, 50);
    root = insert(root, 30);
    root = insert(root, 20);
    root = insert(root, 40);
    root = insert(root, 70);
    root = insert(root, 60);
    root = insert(root, 80);

    cout << "Inorder traversal of the given treap: \n";
    inorder(root);

    return 0;
}
