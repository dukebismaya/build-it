/*
 * Problem Description:
 * Implement Tournament Sort algorithm (Optimized Variant).
 * A variation of selection sort where a tournament tree is built to find the minimum.
 */

#include <iostream>
#include <vector>
#include <limits>

using namespace std;

void tournamentSort(vector<int>& arr) {
    int n = arr.size();
    if (n == 0) return;
    
    int treeSize = 2 * n - 1;
    vector<int> tree(treeSize);
    
    int offset = n - 1;
    for (int i = 0; i < n; i++) {
        tree[offset + i] = i;
    }
    
    for (int i = offset - 1; i >= 0; i--) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (arr[tree[left]] < arr[tree[right]]) {
            tree[i] = tree[left];
        } else {
            tree[i] = tree[right];
        }
    }
    
    vector<int> sorted;
    for (int k = 0; k < n; k++) {
        int minIdx = tree[0];
        sorted.push_back(arr[minIdx]);
        
        arr[minIdx] = numeric_limits<int>::max();
        int curr = offset + minIdx;
        while (curr > 0) {
            int parent = (curr - 1) / 2;
            int left = 2 * parent + 1;
            int right = 2 * parent + 2;
            
            int bestChild = (right < treeSize && arr[tree[right]] < arr[tree[left]]) ? right : left;
            tree[parent] = tree[bestChild];
            
            curr = parent;
        }
    }
    arr = sorted;
}

int main() {
    vector<int> arr = {15, 3, 9, 8, 2, 7, 1, 6};
    cout << "Original Array: ";
    for(int x: arr) cout << x << " ";
    cout << "\n";

    tournamentSort(arr);

    cout << "Sorted Array: ";
    for(int x: arr) cout << x << " ";
    cout << "\n";

    return 0;
}
