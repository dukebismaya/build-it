/*
 * Problem Description:
 * Implement a simple B-Tree search.
 * Given a B-Tree and a key, search for the key in the tree.
 */

#include <iostream>
#include <vector>

using namespace std;

class BTreeNode {
public:
    vector<int> keys;
    int t;
    vector<BTreeNode*> C;
    bool leaf;

    BTreeNode(int _t, bool _leaf) {
        t = _t;
        leaf = _leaf;
    }
    
    BTreeNode* search(int k) {
        int i = 0;
        while (i < keys.size() && k > keys[i])
            i++;

        if (i < keys.size() && keys[i] == k)
            return this;

        if (leaf == true)
            return NULL;

        return C[i]->search(k);
    }
};

class BTree {
public:
    BTreeNode* root;
    int t;

    BTree(int _t) {
        root = NULL;
        t = _t;
    }

    BTreeNode* search(int k) {
        return (root == NULL) ? NULL : root->search(k);
    }
};

int main() {
    BTree t(3);
    t.root = new BTreeNode(3, true);
    t.root->keys = {10, 20, 30};

    int k = 20;
    if (t.search(k) != NULL)
        cout << k << " is present in B-Tree";
    else
        cout << k << " is not present in B-Tree";

    return 0;
}
