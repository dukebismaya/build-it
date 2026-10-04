/*
 * Problem Description:
 * Implement a K-D Tree (k-dimensional tree).
 * A space-partitioning data structure for organizing points in a k-dimensional space.
 */

#include <iostream>
#include <vector>
using namespace std;

const int k = 2;

struct Node {
    int point[k];
    Node *left, *right;
    
    Node(int arr[]) {
        for (int i = 0; i < k; i++)
            point[i] = arr[i];
        left = right = nullptr;
    }
};

class KDTree {
private:
    Node* insertRec(Node* root, int point[], unsigned depth) {
        if (root == nullptr)
            return new Node(point);

        unsigned cd = depth % k;

        if (point[cd] < (root->point[cd]))
            root->left = insertRec(root->left, point, depth + 1);
        else
            root->right = insertRec(root->right, point, depth + 1);

        return root;
    }

    bool searchRec(Node* root, int point[], unsigned depth) {
        if (root == nullptr) return false;

        bool areSame = true;
        for (int i = 0; i < k; ++i) {
            if (root->point[i] != point[i]) {
                areSame = false;
                break;
            }
        }
        if (areSame) return true;

        unsigned cd = depth % k;

        if (point[cd] < root->point[cd])
            return searchRec(root->left, point, depth + 1);

        return searchRec(root->right, point, depth + 1);
    }

public:
    Node* root;
    KDTree() { root = nullptr; }

    void insert(int point[]) {
        root = insertRec(root, point, 0);
    }

    bool search(int point[]) {
        return searchRec(root, point, 0);
    }
};

int main() {
    KDTree tree;
    int points[][k] = {{3, 6}, {17, 15}, {13, 15}, {6, 12}, {9, 1}, {2, 7}, {10, 19}};
    
    int n = sizeof(points)/sizeof(points[0]);

    for (int i=0; i<n; i++)
        tree.insert(points[i]);

    int point1[] = {10, 19};
    (tree.search(point1)) ? cout << "Found\n" : cout << "Not Found\n";

    int point2[] = {12, 19};
    (tree.search(point2)) ? cout << "Found\n" : cout << "Not Found\n";

    return 0;
}
