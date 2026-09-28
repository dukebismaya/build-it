/*
 * Problem Description:
 * Implement Cartesian Tree Sort algorithm.
 * Cartesian tree sort is an adaptive sorting algorithm based on Cartesian trees.
 */

#include <iostream>
#include <vector>
#include <stack>
#include <queue>
using namespace std;

struct Node {
    int value;
    Node* left;
    Node* right;
    Node(int val) : value(val), left(nullptr), right(nullptr) {}
};

Node* buildCartesianTree(const vector<int>& arr) {
    if (arr.empty()) return nullptr;
    stack<Node*> st;
    for (int num : arr) {
        Node* curr = new Node(num);
        Node* lastPopped = nullptr;
        while (!st.empty() && st.top()->value > num) {
            lastPopped = st.top();
            st.pop();
        }
        curr->left = lastPopped;
        if (!st.empty()) {
            st.top()->right = curr;
        }
        st.push(curr);
    }
    while (st.size() > 1) st.pop();
    return st.empty() ? nullptr : st.top();
}

void inOrderTraversal(Node* root, vector<int>& sortedArr) {
    if (root == nullptr) return;
    inOrderTraversal(root->left, sortedArr);
    sortedArr.push_back(root->value);
    inOrderTraversal(root->right, sortedArr);
}

void cartesianTreeSort(vector<int>& arr) {
    Node* root = buildCartesianTree(arr);
    arr.clear();
    inOrderTraversal(root, arr);
}

int main() {
    vector<int> arr = {9, 3, 7, 1, 8, 12, 10, 20, 15, 18, 5};
    cout << "Original Array: ";
    for (int num : arr) cout << num << " ";
    cout << endl;

    cartesianTreeSort(arr);

    cout << "Sorted Array: ";
    for (int num : arr) cout << num << " ";
    cout << endl;
    return 0;
}
