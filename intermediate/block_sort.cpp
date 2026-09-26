/*
 * Problem Description:
 * Implement Block Sort algorithm.
 * Block sort, or block merge sort, is a sorting algorithm with O(n) time complexity for best case and O(n log n) for worst case.
 * It is an in-place and stable sorting algorithm.
 */

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// A simplified block sort concept using built-in std::stable_sort for blocks
void blockSort(vector<int>& arr) {
    int n = arr.size();
    if (n <= 1) return;
    
    int block_size = 3; 
    
    for (int i = 0; i < n; i += block_size) {
        int end = min(i + block_size, n);
        stable_sort(arr.begin() + i, arr.begin() + end);
    }
    
    for (int size = block_size; size < n; size *= 2) {
        for (int left = 0; left < n - size; left += 2 * size) {
            int mid = left + size;
            int right = min(left + 2 * size, n);
            inplace_merge(arr.begin() + left, arr.begin() + mid, arr.begin() + right);
        }
    }
}

int main() {
    vector<int> arr = {5, 2, 9, 1, 5, 6, 3, 8, 4, 7};
    cout << "Original Array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    blockSort(arr);
    
    cout << "Sorted Array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    return 0;
}
