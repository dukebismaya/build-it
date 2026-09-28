/*
 * Problem Description:
 * Implement Circle Sort algorithm.
 * Circle sort is an esoteric sorting algorithm conceptually based on concentric circles.
 */

#include <iostream>
#include <vector>
using namespace std;

bool circleSortRecursive(vector<int>& arr, int low, int high) {
    bool swapped = false;
    if (low == high) return false;
    
    int l = low, h = high;
    while (l < h) {
        if (arr[l] > arr[h]) {
            swap(arr[l], arr[h]);
            swapped = true;
        }
        l++;
        h--;
    }
    
    if (l == h) {
        if (arr[l] > arr[h + 1]) {
            swap(arr[l], arr[h + 1]);
            swapped = true;
        }
    }
    
    int mid = low + (high - low) / 2;
    bool left = circleSortRecursive(arr, low, mid);
    bool right = circleSortRecursive(arr, mid + 1, high);
    
    return swapped || left || right;
}

void circleSort(vector<int>& arr) {
    while (circleSortRecursive(arr, 0, arr.size() - 1)) {
        // keep sorting
    }
}

int main() {
    vector<int> arr = {6, 5, 3, 1, 8, 7, 2, 4};
    cout << "Original Array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    circleSort(arr);
    
    cout << "Sorted Array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    return 0;
}
