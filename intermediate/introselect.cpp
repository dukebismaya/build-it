/*
 * Problem Description:
 * Implement Introselect algorithm.
 * Introselect is a selection algorithm that is a hybrid of quickselect and median of medians.
 */

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int partition(vector<int>& arr, int left, int right, int pivotIndex) {
    int pivotValue = arr[pivotIndex];
    swap(arr[pivotIndex], arr[right]);
    int storeIndex = left;
    for (int i = left; i < right; i++) {
        if (arr[i] < pivotValue) {
            swap(arr[storeIndex], arr[i]);
            storeIndex++;
        }
    }
    swap(arr[right], arr[storeIndex]);
    return storeIndex;
}

int introselect(vector<int>& arr, int left, int right, int k, int depthLimit) {
    if (left == right)
        return arr[left];
        
    if (depthLimit == 0) {
        // Fallback to sorting for guaranteed O(N log N) -> O(N) selection conceptually if using median of medians, here just sorting small chunk
        sort(arr.begin() + left, arr.begin() + right + 1);
        return arr[k];
    }
    
    int pivotIndex = left + (right - left) / 2;
    pivotIndex = partition(arr, left, right, pivotIndex);
    
    if (k == pivotIndex)
        return arr[k];
    else if (k < pivotIndex)
        return introselect(arr, left, pivotIndex - 1, k, depthLimit - 1);
    else
        return introselect(arr, pivotIndex + 1, right, k, depthLimit - 1);
}

int main() {
    vector<int> arr = {9, 1, 0, 4, 6, 2, 7, 5, 8, 3};
    int k = 4;
    int depthLimit = 2 * log2(arr.size());
    
    int kth_element = introselect(arr, 0, arr.size() - 1, k, depthLimit);
    
    cout << "The " << k << "th smallest element is " << kth_element << endl;
    return 0;
}
