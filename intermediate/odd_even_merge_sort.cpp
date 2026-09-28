/*
 * Problem Description:
 * Implement Batcher's Odd-Even Merge Sort.
 * It is a sorting network algorithm that sorts in O(n log^2 n) time.
 */

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void oddEvenMerge(vector<int>& arr, int lo, int n, int r) {
    int m = r * 2;
    if (m < n) {
        oddEvenMerge(arr, lo, n, m);
        oddEvenMerge(arr, lo + r, n, m);
        for (int i = lo + r; i + r < lo + n; i += m) {
            if (arr[i] > arr[i + r])
                swap(arr[i], arr[i + r]);
        }
    } else {
        if (arr[lo] > arr[lo + r])
            swap(arr[lo], arr[lo + r]);
    }
}

void oddEvenMergeSort(vector<int>& arr, int lo, int n) {
    if (n > 1) {
        int m = n / 2;
        oddEvenMergeSort(arr, lo, m);
        oddEvenMergeSort(arr, lo + m, m);
        oddEvenMerge(arr, lo, n, 1);
    }
}

int main() {
    vector<int> arr = {3, 7, 4, 8, 6, 2, 1, 5}; // Power of 2 required for this simple implementation
    cout << "Original Array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    oddEvenMergeSort(arr, 0, arr.size());
    
    cout << "Sorted Array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    return 0;
}
